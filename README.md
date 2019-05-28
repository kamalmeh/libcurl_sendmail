curlmail - libcurl utility to send mail through smtp ssl/tls

Requirements:
	Ubuntu/Dabian:
		sudo apt install libcurl4-gnutls-dev
	Fedora/RedHat:
		sudo dnf install libcurl-devel

Compilation:
	Option1: make
	
	Option2: gcc -o mSend mSend.c -lcurl

	if Option2 does not work then try below option

	Option3: `curl-config --cc --cflags` -o mSend mSend.c `curl-config --libs`

Execution:

	1. Below will be completely non-interactive because we have given all parameters on command line.
	
		<prompt>$ curlmail --to user1@domain.com,user2@domain.com --from user3@domain.com --subject "Test Message" --message "This is test message from the libcurl c program" --user user@domain.com --pass password --email-server smtps://smtp.gmail.com

	2. Below will be interactive mode.
	
		<prompt>$ curlmail

	3. How to Use?
		<prompt>$ curlmail -h
		<prompt>$ curlmail --help

	4. Debug messages:
		<prompt>$ curlmail -v
		<prompt>$ curlmail --verbose
