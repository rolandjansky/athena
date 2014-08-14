package atlantis.utils.xml;

import atlantis.utils.AAtlantisException;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;
import java.io.IOException;
import java.io.InputStream;
import org.xml.sax.EntityResolver;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

/**
 * Entity resolver, used for relaying XML file includes to AUtilities. 
 * AUtilities.getFileAsStream() handles including files that are located in 
 * the webstart jar file.
 *
 * @author Eric Jansen
 */
public class AXMLEntityResolver implements EntityResolver {

    public static ALogger logger = ALogger.getLogger(AXMLEntityResolver.class);
        
    @Override
    public InputSource resolveEntity(String publicId, String systemId) throws SAXException, IOException {
        
        InputStream input = null;
        try {
            if (systemId.startsWith("file://")) {
                systemId = systemId.substring(7);
            }
            input = AUtilities.getFileAsStream(systemId);
        } catch (AAtlantisException e) {

            logger.error("Could not resolve external entity"
                    +" '"+systemId+"': " + e.getMessage());
        }
        
        return new InputSource(input);
    }    
}
