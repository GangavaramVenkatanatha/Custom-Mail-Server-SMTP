# Custom SMTP Server

A Linux-based custom email server implementation developed in C that provides SMTP functionality for sending emails and POP3 functionality for retrieving emails. The project supports user authentication, mailbox management, DKIM-based mail signing, logging, and basic client-server communication over TCP sockets.

---

## Features

### SMTP Server
- User registration
- User authentication
- Send emails between registered users
- Recipient validation
- Mail storage system
- Multiple client handling
- Graceful server shutdown
- Maximum active connection control

### POP3 Server
- User authentication
- Inbox access
- Sent mailbox access
- List available messages
- Retrieve stored emails
- Limited authentication attempts for security

### Security Features
- DKIM-style email signing using OpenSSL
- SHA-256 based digital signatures
- Base64 encoding and decoding
- Authentication validation

### Logging System
- Server activity logging
- Login event tracking
- POP3 authentication logs

---

## Project Structure

```
Custom SMTP Server/
│
├── src/
│   ├── smtp_server.c
│   ├── pop3_server.c
│   ├── auth.c
│   ├── mail.c
│   ├── dkim.c
│   └── logger.c
│
├── include/
│   ├── auth.h
│   ├── mail.h
│   ├── dkim.h
│   └── logger.h
│
├── mailboxes/
│   └── User mail storage
│
├── private.pem
├── public.pem
├── logs.txt
├── users.txt
└── README.md
```

---

## Technologies Used

- C Programming Language
- TCP Socket Programming
- SMTP Protocol Concepts
- POP3 Protocol Concepts
- OpenSSL Cryptographic Library
- Linux System Programming

---

## System Requirements

### Supported Platform

✅ Linux

### Not Supported

❌ Windows

This project uses Linux-specific networking libraries such as:

```c
arpa/inet.h
unistd.h
fork()
```

Therefore, the project is intended to run only on Linux environments.

---

## Dependencies

Install required packages:

Ubuntu/Debian:

```bash
sudo apt update

sudo apt install build-essential openssl libssl-dev netcat
```

Verify GCC installation:

```bash
gcc --version
```

---

## Build Instructions

### Compile SMTP Server

```bash
gcc src/smtp_server.c \
src/auth.c \
src/mail.c \
src/dkim.c \
src/logger.c \
-o smtp_server \
-lssl -lcrypto
```

### Run SMTP Server

```bash
./smtp_server
```

---

### Compile POP3 Server

```bash
gcc src/pop3_server.c \
src/auth.c \
src/mail.c \
src/dkim.c \
src/logger.c \
-o pop3_server \
-lssl -lcrypto
```

### Run POP3 Server

```bash
./pop3_server
```

---

## SMTP Client Commands

Connect to SMTP server:

```bash
nc <server-ip> 2525
```

Register:

```
REGISTER username password
```

Authenticate:

```
AUTH username password
```

Specify sender:

```
MAIL FROM: username
```

Specify receiver:

```
RCPT TO: username
```

Begin email body:

```
DATA
```

Type message:

```
Hello User
This is a test mail.
.
```

Disconnect:

```
QUIT
```

---

## POP3 Client Commands

Connect:

```bash
nc <server-ip> 1100
```

Authenticate:

```
AUTH username password
```

View inbox:

```
MODE INBOX
```

View sent folder:

```
MODE SENT
```

List mails:

```
LIST
```

Retrieve message:

```
RETR 1
```

Exit:

```
QUIT
```

---

## Mail Storage Format

Each user contains:

```
mailboxes/
└── username/
    ├── inbox/
    │   └── mail files
    │
    └── sent/
        └── sent mail files
```

Stored email format:

```
From: sender
To: receiver
DKIM-Signature: <signature>

Message Content
```

---

## Security Mechanism

The project uses OpenSSL to:

- Generate digital signatures
- Verify email authenticity
- Encode signatures using Base64
- Improve integrity validation

---

## Logging

Server events are stored inside:

```
logs.txt
```

Example:

```
[TimeStamp] POP3 Login Success
```

---

## Screenshots
<img width="1916" height="626" alt="Screenshot 2026-05-21 224821" src="https://github.com/user-attachments/assets/80d021f2-2c65-4194-be7a-ea7d8b012319" />
<img width="1919" height="710" alt="Screenshot 2026-05-21 225714" src="https://github.com/user-attachments/assets/4f33f494-652d-4e29-afe6-166563de73df" />
<img width="1919" height="488" alt="Screenshot 2026-05-21 225745" src="https://github.com/user-attachments/assets/0b128357-aa03-44a4-a6ec-ee1864d9ae04" />

# Using GUI
<img width="601" height="86" alt="image" src="https://github.com/user-attachments/assets/ba7cb83a-879e-4a5c-87b4-f7b80d6c314a" />
<img width="1919" height="1117" alt="Screenshot 2026-05-21 230236" src="https://github.com/user-attachments/assets/825b5411-9b50-4d51-8b6f-d700ff915ffa" />

# POP3 Server
<img width="1905" height="368" alt="Screenshot 2026-05-21 231626" src="https://github.com/user-attachments/assets/8e2b6942-72af-496d-81c4-fae3004ef1ac" />
<img width="1909" height="444" alt="Screenshot 2026-05-21 231707" src="https://github.com/user-attachments/assets/96c7c9af-149c-466a-9158-09c2f98432e0" />

--- And here we are getting "error mail tampered" because we sending the mail using name "Venkat" instead of the "Venky". Here due to this dkim signatures did not match and popped it as an error. 
--- If sender or receiver are not registered on the server then we cannot send or recieve the mail from them and it shows error like this.

<img width="1919" height="467" alt="image" src="https://github.com/user-attachments/assets/dd1d59f0-d56a-4bc7-860d-42599c17a973" />

<img width="1074" height="519" alt="Screenshot 2026-05-21 231905" src="https://github.com/user-attachments/assets/2fc83e6e-2ac4-438f-a7ca-2e54310ea94b" />

### NOTE
>>> Note that both server and client should be connected on same wifi or hotspot
>>> And this server is limited to only 100 connections

---

## Future Improvements

- TLS encryption support
- Multi-threaded SMTP processing
- Better POP3 message management
- Email deletion support
- Mail attachment handling
- Persistent database integration

---

## Authors

Developed as a Computer Networks project.

Project Title:

**Custom SMTP Server**

---
