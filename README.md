# TCPX is a  C HTTP Server — A Long-Term Project Inspired by Apache/Nginx

Welcome to the early stages of a long-term project to build a robust, modular, and high-performance HTTP server in C — akin to Apache or Nginx. The goal is to fully understand and implement each layer from scratch, incrementally growing the server into a powerful and flexible web server.

---

## 📌 Description

This project is a foundational implementation of an HTTP server written in C, designed with simplicity, modularity, and extensibility in mind. It supports both raw TCP echo-style communication and basic HTTP request handling.

The codebase is intended to evolve gradually into a full-fledged HTTP server that can handle static files, support concurrency, offer security (TLS/HTTPS), and more — while remaining educational and minimal at each step.


---

## 🚧 Upcoming Features

### 🔜 Next Milestone: **Serving Static Files**

---

## 🛠️ Build & Run

```bash
gcc -o my_http_server server.c -lpthread
./my_http_server <PORT>
