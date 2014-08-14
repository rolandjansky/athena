package atlantis.nge;

/**
 * This class contains some static final boolean flags that can be used to
 * control behaviour such as debugging and sanity checking.
 * 
 * The java compiler guarantees that code enclosed in an if(var) where var
 * is a final and false boolean is entirely excluded from your class file
 * 
 * This is better than just using logger.debug() here because we don't want
 * to do the debug level testing inside the fast rendering path
 * 
 * @author Adam Davison
 */
public class ANGlobalFlags {

}
