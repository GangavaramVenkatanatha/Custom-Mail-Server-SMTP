import tkinter as tk
from tkinter import scrolledtext
import subprocess
import threading
import signal
import os

smtp_process = None
pop3_process = None
users = set()

def start_smtp():
    global smtp_process

    if smtp_process is None:
        smtp_process = subprocess.Popen(
            ["./smtp_server"],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            preexec_fn=os.setsid
        )

        smtp_status.config(text="RUNNING", fg="lime")
        log("[SYSTEM] SMTP Server Started\n", "system")

        threading.Thread(target=read_logs, args=(smtp_process,), daemon=True).start()


def stop_smtp():
    global smtp_process, users

    if smtp_process:
        os.killpg(os.getpgid(smtp_process.pid), signal.SIGINT)
        smtp_process.wait()

        smtp_process = None

        users.clear()
        update_users()

        smtp_status.config(text="STOPPED", fg="red")
        client_label.config(text="Active Clients: 0")

        log("[SYSTEM] SMTP Server Stopped\n", "error")


def start_pop3():
    global pop3_process

    if pop3_process is None:
        pop3_process = subprocess.Popen(
            ["./pop3_server"],
            stdout=subprocess.PIPE,
            stderr=subprocess.STDOUT,
            text=True,
            preexec_fn=os.setsid
        )

        pop3_status.config(text="RUNNING", fg="lime")
        log("[SYSTEM] POP3 Server Started\n", "system")

        threading.Thread(target=read_logs, args=(pop3_process,), daemon=True).start()


def stop_pop3():
    global pop3_process

    if pop3_process:
        os.killpg(os.getpgid(pop3_process.pid), signal.SIGINT)
        pop3_process.wait()

        pop3_process = None

        pop3_status.config(text="STOPPED", fg="red")
        client_label.config(text="Active Clients: 0")

        log("[SYSTEM] POP3 Server Stopped\n", "error")


def read_logs(process):
    global users

    for line in process.stdout:

        if "[LOGIN]" in line:
            user = line.split("]")[1].split()[0]
            users.add(user)
            update_users()
            log(line, "login")

        elif "[LOGOUT]" in line:
            user = line.split("]")[1].split()[0]
            users.discard(user)
            update_users()
            log(line, "logout")

        elif "[CLIENTS]" in line:
            count = line.split(":")[1].strip()
            client_label.config(text=f"Active Clients: {count}")
            log(line, "clients")

        else:
            log(line, "normal")


def update_users():
    user_list.delete(0, tk.END)
    for u in users:
        user_list.insert(tk.END, u)


def log(msg, tag="normal"):
    log_area.insert(tk.END, msg, tag)
    log_area.see(tk.END)


root = tk.Tk()
root.title("Mail Server Dashboard")
root.geometry("1100x720")
root.configure(bg="#1e1e1e")

title = tk.Label(root, text="MAIL SERVER DASHBOARD",
                 font=("Arial", 24, "bold"),
                 bg="#1e1e1e", fg="white")
title.pack(pady=10)

top_frame = tk.Frame(root, bg="#1e1e1e")
top_frame.pack()

smtp_frame = tk.Frame(top_frame, bg="#2c2c2c", padx=20, pady=10)
smtp_frame.grid(row=0, column=0, padx=20)

tk.Label(smtp_frame, text="SMTP SERVER", fg="white",
         bg="#2c2c2c", font=("Arial", 14, "bold")).pack()

smtp_status = tk.Label(smtp_frame, text="STOPPED",
                       fg="red", bg="#2c2c2c",
                       font=("Arial", 12, "bold"))
smtp_status.pack()

tk.Button(smtp_frame, text="START SMTP",
          bg="#4CAF50", fg="white", width=15,
          command=start_smtp).pack(pady=5)

tk.Button(smtp_frame, text="STOP SMTP",
          bg="#E53935", fg="white", width=15,
          command=stop_smtp).pack(pady=5)


pop3_frame = tk.Frame(top_frame, bg="#2c2c2c", padx=20, pady=10)
pop3_frame.grid(row=0, column=1, padx=20)

tk.Label(pop3_frame, text="POP3 SERVER", fg="white",
         bg="#2c2c2c", font=("Arial", 14, "bold")).pack()

pop3_status = tk.Label(pop3_frame, text="STOPPED",
                       fg="red", bg="#2c2c2c",
                       font=("Arial", 12, "bold"))
pop3_status.pack()

tk.Button(pop3_frame, text="START POP3",
          bg="#4CAF50", fg="white", width=15,
          command=start_pop3).pack(pady=5)

tk.Button(pop3_frame, text="STOP POP3",
          bg="#E53935", fg="white", width=15,
          command=stop_pop3).pack(pady=5)


client_label = tk.Label(root,
                        text="Active Clients: 0",
                        font=("Arial", 14),
                        fg="white",
                        bg="#1e1e1e")
client_label.pack(pady=5)

main = tk.Frame(root, bg="#1e1e1e")
main.pack(fill="both", expand=True)

log_frame = tk.Frame(main, bg="#1e1e1e")
log_frame.pack(side="left", fill="both", expand=True, padx=10)

tk.Label(log_frame, text="SERVER LOGS",
         fg="white", bg="#1e1e1e",
         font=("Arial", 14, "bold")).pack()

log_area = scrolledtext.ScrolledText(log_frame,
                                     bg="black",
                                     fg="white",
                                     font=("Courier", 12))
log_area.pack(fill="both", expand=True)

log_area.tag_config("login", foreground="lime")
log_area.tag_config("logout", foreground="red")
log_area.tag_config("clients", foreground="yellow")
log_area.tag_config("system", foreground="cyan")
log_area.tag_config("error", foreground="red")
log_area.tag_config("normal", foreground="white")


user_frame = tk.Frame(main, bg="#1e1e1e")
user_frame.pack(side="right", fill="y", padx=10)

tk.Label(user_frame, text="ACTIVE USERS",
         fg="white", bg="#1e1e1e",
         font=("Arial", 14, "bold")).pack()

user_list = tk.Listbox(user_frame,
                       bg="black",
                       fg="cyan",
                       font=("Courier", 12),
                       width=25)
user_list.pack(fill="y")

root.mainloop()
