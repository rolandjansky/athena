package atlantis.event.xmlrpc;

import atlantis.utils.*;


/**
 *
 * Exception thrown after errors occured during calls
 * to remote server methods (within Athena) via XMLRPC
 *
 * @author Zdenek Maxa
 */
public class ARemoteCallerException extends AAtlantisException
{
    public ARemoteCallerException(String msg)
    {
        super("Exception while calling remote procedure.\n" + msg);

    } // ARemoteCallerException() -------------------------------------------


} // class ARemoteCallerException ===========================================
