all:
	gcc -static -o cgi-bin/index.cgi cgi-bin/client.c

start:
	@busybox httpd -p 8000
	@echo "HTTP server started at localhost:8000"
	@./server-example.sh
