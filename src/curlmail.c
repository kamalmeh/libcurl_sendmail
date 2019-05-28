/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/ 
 
/* <DESC>
 * SMTP example using SSL
 * </DESC>
 */ 
 
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>    /* for exit */
#include <getopt.h>
#include <curl/curl.h>
#include <string.h>
 
/* This is a simple example showing how to send mail using libcurl's SMTP
 * capabilities. It builds on the smtp-mail.c example to add authentication
 * and, more importantly, transport security to protect the authentication
 * details from being snooped.
 *
 * Note that this example requires libcurl 7.20.0 or above.
 */ 
 
/*Default Values*/
#define SMTP_USER "user@email.com"
#define SMTP_PASS "useremail_password"
#define SMTP_SERVER "smtps://smtp.gmail.com"
#define EMAIL_SUBJECT "TEST SUBJECT"
#define EMAIL_BODY "TEST EMAIL BODY MESSAGE"
#define MAX_SIZE 255

/*Initialize to null strings to avoid segmentation fault errors in string
 * funtions and other places*/
char *from="\0",*to="\0",*cc="\0",*bcc="\0",*user="\0",*pass="\0";
char *subject="\0",*message="\0",*smtp_server="\0",*attachment="\0";
char *program = "\0";

/*DEBUG is disabled by default*/
int verbose=0;

char* getInput(const char *prompt){
	printf("%s",prompt);
	char *input=malloc(MAX_SIZE);
     	fflush(stdin);
	fgets(input,MAX_SIZE,stdin);
/* START - Remove newline character from the stadard input*/
	size_t ln = strlen(input) - 1;
	if (*input && input[ln] == '\n') 
    		input[ln] = '\0';
/* END - Remove newline character from the stadard input*/

	return input;
}

void usage(void)
{
	printf("Description: This is a sendmail program using curl library 'libcurl'\n");
	printf("\n");
	printf("USAGE: %s [Options]\n",program);	
	printf("Options:\n");
	printf("%-30s\t%-80s\n\t\t\t\t\t%-80s\n","\t-f | --from","Optional. Sender Email. If not entered \
on command line, program enters into interactive mode.","Max length \
of the value is 255 characters. Default: SMTP User value");
	printf("%-30s\t%-80s\n\t\t\t\t\t%-80s\n","\t-t | --to","Required. Recipient's Email. If not entered \
on command line, program enters into interactive mode.","Max length \
of the value is 255 characters. Multiple recipient can be provided comma separated.");
	printf("%-30s\t%-80s\n\t\t\t\t\t%-80s\n","\t-c | --cc","Optional. CC Recipient's Email. If not entered \
on command line, program enters into interactive mode.","Max length \
of the value is 255 characters. Multiple recipient can be provided comma separated.");
	printf("%-30s\t%-80s\n\t\t\t\t\t%-80s\n","\t-b | --bcc","Optional. BCC Recipient's Email. If not entered \
on command line, program enters into interactive mode.","Max length \
of the value is 255 characters. Multiple recipient can be provided comma separated.");
	printf("%-30s\t%-80s\n\t\t\t\t\t%-80s\n","\t-s | --subject","Optional. Email Subject. If not entered \
on command line, program enters into interactive mode.","Max length of the value is 255 characters.\
Default: TEST SUBJECT");
	printf("%-30s\t%-80s\n\t\t\t\t\t%-80s\n","\t-m | --message","Optional. Email Message Body. If not entered \
on command line, program enters into interactive mode.","Max length of the value is 255 characters.\
Default: TEST EMAIL BODY MESSAGE");
	printf("%-30s\t%-80s\n\t\t\t\t\t%-80s\n","\t-a | --attachment","Optional. Path of the attachment(s). If not entered \
on command line, program enters into interactive mode.","Max length of the value is 255 characters. Multiple recipient \
can be provided comma separated.");
	printf("%-30s\t%-80s\n\t\t\t\t\t%-80s\n","\t-e | --email-server","Optional. SMTP Server FQDN. If not entered \
on command line, program enters into interactive mode.","Max length of the value is 255 characters.\
Default: smtps://smtp.gmail.com");
	printf("%-30s\t%-80s\n\t\t\t\t\t%-80s\n","\t-u | --user","Optional. Sender's SMTP User. Generally, it is \
Sender's Email. If not entered on command line, program enters into interactive mode.","Max length of the \
value is 255 characters.");
	printf("%-30s\t%-80s\n\t\t\t\t\t%-80s\n","\t-p | --pass","Optional. Sender's SMTP Secret. Generally, it is \
Sender's Email's Password. If not entered on command line, program enters into interactive mode.","Max length of the \
value is 255 characters.");
	printf("%-30s\t%-80s\n","\t-h | --help","Displays usage of this program."); 
	printf("%-30s\t%-80s\n","\t-v | --verbose","Displays verbose messages for debugging of this program."); 
	printf("\n");
	exit(1);
}

int parse_arguments(int argc, char *argv[])
{
     int c;
     program=strrchr(argv[0],'/');
     program++;

     while (1) {
          int option_index = 0;
          static struct option long_options[] = {
               {"from",		required_argument,	0,  	102},
               {"to",		required_argument,	0,  	116},
               {"cc",		required_argument,	0,  	99},
               {"bcc",		required_argument,	0,  	98},
               {"user",		required_argument,	0,  	117},
               {"pass",		required_argument,	0,  	112},
               {"subject",	required_argument,	0,	115},
               {"message",	required_argument,	0,	109},
               {"email-server",	required_argument,	0,	101},
               {"attachment",	optional_argument,	0,	97},
               {"help",		no_argument,		0,	104},
               {"verbose",	no_argument,		0,	118},
	       {0,0,0,0}
          };

          c = getopt_long(argc, argv, "hvf:t:c:b:u:p:s:m:a:", long_options, &option_index);
          if (c == -1)
               break;

          switch (c) {
               case 102:
                    from=optarg;
                    break;

               case 101:
                    smtp_server=optarg;
                    break;

               case 116:
                    to=optarg;
                    break;

               case 115:
                    subject=optarg;
                    break;

               case 98:
                    bcc=optarg;
                    break;

               case 99:
                    cc=optarg;
                    break;

               case 109:
                    message=optarg;
                    break;

               case 112:
                    pass=optarg;
                    break;

               case 97:
	               if(optarg){
                         attachment=optarg;
	               }
                    break;

               case 117:
                    user=optarg;
                    break;
               
	       case 118:
                    verbose=1;
                    break;

               case 104:
		    usage();
                    break;

               default:
		    exit(2);
          }
     }

     if (optind < argc) {
          printf("Unknown-option ARGV-elements: ");
          while (optind < argc)
               printf("%s ", argv[optind++]);
          printf("\n");
     }
     return(EXIT_SUCCESS);

}

int main(int argc, char *argv[])
{
     CURL *curl;
     CURLcode res = CURLE_OK;
     struct curl_slist *recipients = NULL;
     struct curl_slist *headers = NULL;
     struct curl_slist *slist = NULL;
     curl_mime *mime;
     curl_mime *alt;
     curl_mimepart *part;

/* Parse Arguments */
     parse_arguments(argc, argv);
     if(strlen(user)==0){
         user=SMTP_USER;
     }
     if(verbose==1)
         printf("SMTP User Name='%s'\n", user);
     if(strlen(from)==0)
	     from=SMTP_USER;

/* Recipient address is required to send mail.
     if(strlen(to)==0)
	     to=getInput("Enter Recipient Email(Comma Separated in case of multiple. Max length 255 Characters): ");
/*
     ans=getInput("Do you want to add CC? (Y|n): ");
     if(ans[0]=='Y'){
     		if(strlen(cc)==0)
	     		cc=getInput("Enter CC Email(Comma Separated in case of multiple. Max length 255 Characters): ");
     }
     ans=getInput("Do you want to add BCC? (Y|n): ");
     if(ans[0]=='Y'){
     		if(strlen(bcc)==0)
	     		bcc=getInput("Enter BCC Email(Comma Separated in case of multiple. Max length 255 Characters): ");
     }
*/
     if(strlen(subject)==0)
	     subject=EMAIL_SUBJECT;
     if(strlen(message)==0)
	     message=EMAIL_BODY;
/*
     ans=getInput("Do you want to attach any file(s)? (Y|n): ");
     if(ans[0]=='Y'){
     		if(strlen(attachment)==0)
	     		attachment=getInput("Enter attachment absolute comma separated path(s): ");
     }
*/
     if(strlen(smtp_server)==0)
	     smtp_server=SMTP_SERVER;



     if(verbose==1){
         printf("Option --from='%s'\n", from);
         printf("Option --to='%s'\n", to);
         printf("Option --cc='%s'\n", cc);
         printf("Option --bcc='%s'\n", bcc);
         printf("Option --subject='%s'\n", subject);
         printf("Option --message='%s'\n", message);
	 printf("Option --attachment='%s'\n",attachment);
	 printf("Option --user='%s'\n", user);
         printf("Option --email-server='%s'\n", smtp_server);
     }

/* Time */
     time_t rawtime;
     struct tm * timeinfo;
     char time_buffer[128];
     time(&rawtime);
     timeinfo = localtime (&rawtime);
     strftime (time_buffer, 128, "%a, %d %b %Y %H:%M:%S %z", timeinfo);
/* END Time */

     const char payload_template[] =
          "Date: %s\r\n"
          "To: %s\r\n"
          "From: %s\r\n"
          "Message-ID: <%s>\r\n"
          "Subject: %s\r\n"
          "\r\n";

     char *message_id = from;

     size_t payload_text_len = strlen(payload_template) +  
                                  strlen(time_buffer) +
                                  strlen(to) + strlen(from) +
                                  strlen(subject) +
                                  strlen(message_id) + 1;

     char *payload_text = malloc(payload_text_len);
     memset(payload_text, 0, payload_text_len);
     sprintf(payload_text, payload_template, time_buffer, to, from, message_id,
		     subject);
     if(verbose==1)
     	printf("Headers: %s\n",payload_text);

     static const char inline_text[] = "%s\r\n";
     size_t text_message_size=strlen(inline_text)+strlen(message)+10;
     char *text_msg=malloc(text_message_size);
     memset(text_msg, 0, text_message_size);
     sprintf(text_msg, inline_text, message);

     static const char inline_html[] = "%s\r\n";
     size_t html_message_size=strlen(inline_html)+strlen(message)+10;
     char *html_msg=malloc(html_message_size);
     memset(html_msg, 0, html_message_size);
     sprintf(html_msg, inline_html, message);

     curl = curl_easy_init();
     if(curl) {
/* Set username and password */ 
          if(strlen(pass)==0){
	       pass=SMTP_PASS;
	  }

          curl_easy_setopt(curl, CURLOPT_USERNAME, user);
          curl_easy_setopt(curl, CURLOPT_PASSWORD, pass);
 
/* This is the URL for your mailserver. Note the use of smtps:// rather
* than smtp:// to request a SSL based connection. */
          curl_easy_setopt(curl, CURLOPT_URL, smtp_server);
 
/* If you want to connect to a site who isn't using a certificate that is
* signed by one of the certs in the CA bundle you have, you can skip the
* verification of the server's certificate. This makes the connection
* A LOT LESS SECURE.
*
* If you have a CA cert for the server stored someplace else than in the
* default bundle, then the CURLOPT_CAPATH option might come handy for
* you. */ 
#ifdef SKIP_PEER_VERIFICATION
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
#endif
 
/* If the site you're connecting to uses a different host name that what
* they have mentioned in their server certificate's commonName (or
* subjectAltName) fields, libcurl will refuse to connect. You can skip
* this check, but this will make the connection less secure. */ 
#ifdef SKIP_HOSTNAME_VERIFICATION
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
#endif
    curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
 
/* Note that this option isn't strictly required, omitting it will result
* in libcurl sending the MAIL FROM command with empty sender data. All
* autoresponses should have an empty reverse-path, and should be directed
* to the address in the reverse-path which triggered them. Otherwise,
* they could cause an endless loop. See RFC 5321 Section 4.5.5 for more
* details.
*/
          if(strlen(from)==0){
	       from=SMTP_USER;
	  }
          curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);
 
/* Add two recipients, in this particular case they correspond to the
* To: and Cc: addressees in the header, but they could be any kind of
* recipient. */ 
          if(strlen(to)==0)
	       to=getInput("Enter Recipient Email: ");
	  char* token;
    	  char* rest = to;
    	  while ((token = strtok_r(rest, ",", &rest)))
          	recipients = curl_slist_append(recipients, token);

          if(strlen(cc)>0){
	  	char* token;
    	  	char* rest = cc;
    	  	while ((token = strtok_r(rest, ",", &rest)))
          		recipients = curl_slist_append(recipients, token);
	  }
          if(recipients)    
               curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

/* Build and set the message header list. */
          headers = curl_slist_append(headers, payload_text);
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

/* Build the mime message. */ 
          mime = curl_mime_init(curl);
 
/* The inline part is an alternative proposing the html and the text
versions of the e-mail. */ 
          alt = curl_mime_init(curl);
 
/* HTML message. */ 
          part = curl_mime_addpart(alt);
          curl_mime_data(part, html_msg, CURL_ZERO_TERMINATED);
          curl_mime_type(part, "text/html");
 
/* Text message. */ 
          part = curl_mime_addpart(alt);
          curl_mime_data(part, text_msg, CURL_ZERO_TERMINATED);
 
/* Create the inline part. */ 
          part = curl_mime_addpart(mime);
          curl_mime_subparts(part, alt);
          curl_mime_type(part, "multipart/alternative");
          slist = curl_slist_append(NULL, "Content-Disposition: inline");
          curl_mime_headers(part, slist, 1);
 
/* Add all attachments. */ 
	  if(strlen(attachment)>0){
	  	char* token;
    	  	char* rest = attachment;
    	  	while ((token = strtok_r(rest, ",", &rest))){
          		part = curl_mime_addpart(mime);
			curl_mime_filedata(part, token);
        		curl_mime_encoder(part, "base64");
		}
	  }
          curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
          curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

/* Since the traffic will be encrypted, it is very useful to turn on debug
* information within libcurl to see what is happening during the
* transfer */ 
	  if(verbose==1)
          	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
	  else
          	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0L);
 
/* Send the message */ 
          res = curl_easy_perform(curl);
 
/* Check for errors */ 
          if(res != CURLE_OK)
               fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
 
/* Free the list of recipients */ 
          curl_slist_free_all(recipients);
 
/* Always cleanup */ 
          curl_easy_cleanup(curl);
     }
 
     return (int)res;
}
