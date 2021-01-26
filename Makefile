all: sshell

sshell: sshell.c
	gcc -Wall -Wextra -o sshell sshell.c

clean:
	rm -f sshell
