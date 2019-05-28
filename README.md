<H1>
curlmail - libcurl utility to send mail through smtp ssl/tls
</H1>

<h1>Requirements</h1>
	<h2>Ubuntu/Dabian:</h2><br>
		<ul><li>sudo apt install libcurl4-gnutls-dev</li></ul>
	<h2>Fedora/RedHat:</h2><br>
		<ul><li>sudo dnf install libcurl-devel</li></ul>

<h1>Compilation</h1>
	<ul>
	<li>Option1: make</li>	
	<li>Option2: gcc -o curlmail curlmail.c -lcurl </li>
	if Option2 does not work then try below option
	<li>Option3: `curl-config --cc --cflags` -o curlmail curlmail.c `curl-config --libs`</li>
	</ul>

<h1>Execution:</h1>

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
