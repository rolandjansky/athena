package atlantis.utils;

/**
 * 
 * @author Zdenek Maxa
 */
public class AAtlantisException extends Exception
{
    private boolean isFatal = true;
    
    // private static ALogger logger = ALogger.getLogger(AAtlantisException.class);


    public AAtlantisException(String msg)
    {
        super(msg);
        // logger.debug(msg, this);
        this.isFatal = true;

    } // AAtlantisException() -----------------------------------------------



    public AAtlantisException(String msg, boolean isFatal)
    {
        super(msg);
        // logger.debug(msg, this);
        this.isFatal = isFatal;

    } // AAtlantisException() -----------------------------------------------



    public boolean isFatal()
    {
        return this.isFatal;

    } // isFatal() ----------------------------------------------------------


} // class AAtlantisException ===============================================
