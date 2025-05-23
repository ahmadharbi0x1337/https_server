# 🔧 C HTTP Server — A Long-Term Project Inspired by Apache/Nginx

Welcome to the early stages of a long-term project to build a robust, modular, and high-performance HTTP server in C — akin to Apache or Nginx. The goal is to fully understand and implement each layer from scratch, incrementally growing the server into a powerful and flexible web server.

---

## 📌 Description

This project is a foundational implementation of an HTTP server written in C, designed with simplicity, modularity, and extensibility in mind. It supports both raw TCP echo-style communication and basic HTTP request handling.

The codebase is intended to evolve gradually into a full-fledged HTTP server that can handle static files, support concurrency, offer security (TLS/HTTPS), and more — while remaining educational and minimal at each step.

---

## ✅ Current Progress

### ✔️ Features Implemented So Far:
- ✅ **Socket creation and binding**
- ✅ **Listening and accepting client connections**
- ✅ **Basic logging macros for info and error tracking**
- ✅ **Raw TCP echo server (responds to basic messages)**
  ✅ **Accepting connections**
  ✅ **Parsing basic HTTP requests**
  ✅ **Serving static files from a directory**
- ✅ **Graceful error handling and socket cleanup**

---

## 🚧 Upcoming Features

### 🔜 Next Milestone: **Serving Static Files**

Planned enhancements:
- 🧠 Basic MIME type detection
- 📦 Modular structure for handling HTTP routes and file resolution
- ⛔ Proper error responses for missing or restricted files

---

## 🛠️ Build & Run

```bash
gcc -o my_http_server server.c -lpthread
./my_http_server <PORT>
