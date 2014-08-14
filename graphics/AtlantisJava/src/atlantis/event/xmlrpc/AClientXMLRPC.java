package atlantis.event.xmlrpc;

import atlantis.utils.*;
import java.net.URL;
import java.net.MalformedURLException;
import java.util.Vector;

import org.apache.xmlrpc.client.XmlRpcClient;
import org.apache.xmlrpc.client.AsyncCallback;
import org.apache.xmlrpc.XmlRpcException;
import org.apache.xmlrpc.client.XmlRpcClientConfigImpl;
import org.apache.xmlrpc.client.TimingOutCallback;


/**
 * Implementation of XMLRPC client communicating to the
 * server side running within the Athena framework - JiveXML C++ ulxmlrpcpp server,
 * Python Interactive server.
 *
 * @author Zdenek Maxa
 */
public class AClientXMLRPC
{
	private final static ALogger logger = ALogger.getLogger(AClientXMLRPC.class);

    private String methodName = "";
    private AsyncCallback caller = null;

    // timeout - default timeout delay for synchronous calls [ms]
    private int timeout = 6000;



    
    /**
     * Constructor for synchronous XMLRPC calls
     * @param method
     */
    public AClientXMLRPC(String method)
    {
        this.methodName = method;
        this.caller = null; // synchronous calls will be demanded

    } // AClientXMLRPC() ----------------------------------------------------

    

    /**
     * Constructor for asynchronous XMLRPC calls
     * @param method
     * @param caller
     */
    public AClientXMLRPC(String method, AsyncCallback caller)
    {
        this.methodName = method;
        this.caller = caller; // asynchronous calls

    } // AClientXMLRPC() ----------------------------------------------------

    

    public void setTimeout(int value)
    {
        this.timeout = value;

    } // setTimeout() -------------------------------------------------------

    

    /**
     * Asynchronous XMLRPC call, returns immediately and callback
     * caller is later invoked when result is available.
     * @param client
     * @param params
     * @throws ARemoteCallerException
     */
    private void asynchronousCallXMLRPCServer(XmlRpcClient client, Vector params)
                                             throws ARemoteCallerException
    {
    	String msg = "";
    	
    	logger.debug("Asynchronous XMLRPC call ...");
    	try
    	{
        	client.executeAsync(methodName, params, caller);
        	
    	}
    	catch(Throwable t)
    	{
    		if(t instanceof XmlRpcException)
    		{
    			msg = "XMLRPC exception occured, performing the remote call failed.";
    		}
    		else
    		{
    			msg = "Unspecified exception occured, reason: " + t.getMessage(); 
    		}
			logger.debug(msg, t);
			throw new ARemoteCallerException(msg);    		
    	}
    	
    } // asynchronousCallXMLRPCServer() -------------------------------------
    
    
    
    /**
     * Synchronous XMLRPC call. Implemented as asynchronous but timing-out
     * call so that we wait until the result is available. This way it prevents
     * Atlantis from hanging if remote server (Athena) crashes.
     * @param client
     * @param params
     * @return
     * @throws ARemoteCallerException
     */
    private String synchronousCallXMLRPCServer(XmlRpcClient client, Vector params)
                                            throws ARemoteCallerException
    {
    	String msg = "";
    	String result = null;
    	
    	logger.debug("Synchronous timing-out XMLRPC call (timeout " +
    			     timeout / 1000 + "s) ...");
    	
    	TimingOutCallback callback = new TimingOutCallback(timeout);
    	try
    	{
    		client.executeAsync(methodName, params, callback);
    	}
    	catch(XmlRpcException xmle)
    	{
    		msg = "XMLRPC exception occured, performing the remote call failed.";
			logger.debug(msg, xmle);
			throw new ARemoteCallerException(msg);
    	}
    	
    	
    	try
    	{	
    		result = (String) callback.waitForResponse();
            
    		// no error, result should be event data - all well
        	logger.debug("Performing timing out XMLRPC call successful.");
    	}
    	catch(Throwable t)
    	{
    		if(t instanceof InterruptedException)
    		{
    			msg = "The request was interrupted, timed out after " +
    			    timeout / 1000 + "s, try increasing the timer.";
    		}
    		else if(t instanceof TimingOutCallback.TimeoutException)
    		{
    			msg = "No response was received after waiting " +
    			      timeout / 1000 + "s, try increasing the timer.";
    		}
    		else
    		{
    			msg = "An error was returned by the server, reason: " + t.getMessage(); 
    		}
			logger.debug(msg, t);
			throw new ARemoteCallerException(msg);
    	}
    	
    	return result;
    	
    } // synchronousCallXMLRPCServer() --------------------------------------
    
    

    /**
     * Main method calling remote server methods via XMLRPC
     * 
     * @param serverName - name of the XMLRPC server to call method on
     * @param port - where XMLRPC runs
     * @param params - Vector containing either command to execute,
     *        name of a remote variable to get its value or expression to
     *        process (to change some variables's value)
     * @return String - result
     * @throws ARemoteCallerException 
     */
    public String callXMLRPCServer(String serverName, int port, Vector params)
                                    throws ARemoteCallerException
    {

        XmlRpcClient client = null;
        String result = null;
        String msg = null; // logging temporary variable

        msg = "Calling " + serverName + ":" + port;
        logger.info(msg);

        
        // create instance of XMLRPC client
        try
        {
            // Vector type is only acceptable for XmlRpcClient.execute() /
            // .executeAsync()
            // String is expected on the remote side by C++ ulxmlrpcpp server
            // String, among others, is accepted by Python server as well

            // throws MalformedURLException
        	XmlRpcClientConfigImpl config = new XmlRpcClientConfigImpl();
        	config.setServerURL(new URL(serverName + ":" + port));
        	client = new XmlRpcClient();
        	client.setConfig(config);
        	logger.debug("XMLRPC client instantiated successfully.");
        }
        catch(Throwable t)
        {
            if(t instanceof MalformedURLException)
            {
                msg = "Could not create XMLRPC client, bad address  " +
                      serverName + ":" + port + "\n" +
                      "reason: " + t.getMessage();
            }
            else
            {
                msg = "Unspecified exception occured\n" +
                      "reason: " + t.getMessage();
            }

            logger.debug(msg, t);
            throw new ARemoteCallerException(msg);
        }

                
        if(caller != null)
        {
        	// perform asynchronous XMLRPC call, returns immediately
        	asynchronousCallXMLRPCServer(client, params);
        }
        else
        {
        	// synchronous call, timing out synchronous call
        	result = synchronousCallXMLRPCServer(client, params);
        }
        
    	client =  null;
    	return result;
        
    } // callXMLRPCServer() -------------------------------------------------


} // class AClientXMLRPC ====================================================