# Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration

def sendEmail(addresses,subject,senderaddress,bodytext,server='localhost',replyto=None):
    assert len(addresses)>0
    if replyto:
        replyto='Reply-To: %s\n'%replyto
    else:
        replyto = ''
    message = "From: %s\n%sTo: %s\nSubject: %s\n\n%s\n" % (senderaddress, replyto,", ".join(addresses), subject, bodytext)
    # Send the mail
    try:
        import smtplib
        server = smtplib.SMTP(server)
        server.sendmail(senderaddress, addresses, message)
        server.quit()
    except:
        print "ERROR: Problems sending email!"
        return False

    return True
