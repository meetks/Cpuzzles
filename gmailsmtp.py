
import smtplib
gmail_user =
gmail_pwd = 
FROM = gmail_user
TO = 
#TO = "b@gmail.com", "g@gmail.com"
SUBJECT = "Yo"
TEXT = "LO"

# Prepare actual message
message = """\From: %s\nTo: %s\nSubject: %s\n\n%s
""" % (FROM, ", ".join(TO), SUBJECT, TEXT)
try:
	server = smtplib.SMTP_SSL("smtp.gmail.com","465")
	server.ehlo()
	server.login(gmail_user,gmail_pwd)
	server.sendmail(FROM, TO, message)
	server.close()
        print 'successfully sent the mail'
except:
  print "failed to send mail"
