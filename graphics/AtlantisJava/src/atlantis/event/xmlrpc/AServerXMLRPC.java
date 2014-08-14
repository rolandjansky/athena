package atlantis.event.xmlrpc;

import atlantis.utils.*;
import java.io.IOException;
import org.apache.xmlrpc.XmlRpcException;
import org.apache.xmlrpc.webserver.WebServer;
import org.apache.xmlrpc.server.XmlRpcServer;
import org.apache.xmlrpc.server.PropertyHandlerMapping;

/**
 *
 * @author Adam Davison
 */
public class AServerXMLRPC {

    private static ALogger logger = ALogger.getLogger(AServerXMLRPC.class);
    
    private WebServer m_ws;
    
    public AServerXMLRPC(int port, Class handler) throws XmlRpcException {
        
        logger.debug("Attempting to create XMLRPC server on port " + port);
        logger.debug("Serving requests to handler of type " + handler.getName());
        
        m_ws = new WebServer(port);
        XmlRpcServer xmlRpcServer = m_ws.getXmlRpcServer();
        PropertyHandlerMapping phm = new PropertyHandlerMapping();
        phm.addHandler(handler.getName(), handler);
        xmlRpcServer.setHandlerMapping(phm);
        
        logger.debug("XMLRPC server created successfully");
       
    }
    
    public void start() throws IOException {
        
        logger.debug("Attempting to start XMLRPC server");
        m_ws.start();
        logger.info("XMLRPC server started");
        
    }
    
}
