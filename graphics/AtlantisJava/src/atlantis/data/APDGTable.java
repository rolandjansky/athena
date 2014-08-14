package atlantis.data;

import atlantis.globals.AGlobals;
import atlantis.utils.ALogger;
import atlantis.utils.AUtilities;
import atlantis.utils.xml.*;
import java.util.*;
import java.io.File;
import java.io.InputStream;
import org.w3c.dom.*;
import javax.xml.parsers.*;

/**
 * Read a PDG table from the file pdg.xml and provide 
 * basic accessor functions to its particles
 */
public final class APDGTable {
  
  //The logger
  private final static ALogger logger = ALogger.getLogger(APDGTable.class);

  //Wether to validate the XML document
  private final static boolean VALIDATION=true;
  
  //The particle table linked by PDG code
  private final static Map<Integer, AParticle> particles = new HashMap<Integer, AParticle>(100);

  //Always read in the Hashmap
  static {
    readPDG(APDGTable.getFileInCurrentUserThenAtlantisHome("configuration","pdg.xml"));
  }

  //Read in the PDG table from a file
  private static void readPDG(String fileName) {
    try {
      DocumentBuilderFactory factory=DocumentBuilderFactory.newInstance();

      factory.setValidating(VALIDATION);
      DocumentBuilder parser=factory.newDocumentBuilder();

      parser.setErrorHandler(new AXMLErrorHandler());
      InputStream is = AUtilities.getFileAsStream(fileName);
      Node pdg=parser.parse(is).getDocumentElement();
      NodeList particlesList=pdg.getChildNodes();
      int particlesCount=particlesList.getLength();

      for(int i=0; i<particlesCount; i++) {
        Node particle=particlesList.item(i);

        if(particle.getNodeType()==Node.ELEMENT_NODE) {
          AParticle p=new AParticle(particle);

          particles.put(new Integer(p.getCode()), p);
        }
      }
    } catch(Exception e) {
      logger.error("Failed reading PDG table", e);
    }
  }

  public static int getCharge(int code) {
    int sign=code/Math.abs(code);
    AParticle p=particles.get(new Integer(Math.abs(code)));

    if(p!=null)
      return p.getCharge()*sign;
    else throw new ParticleNotFoundError(code);
  }

  public static String getName(int code) {
    AParticle p=particles.get(new Integer(Math.abs(code)));
    int sign=1;

    if(code<0) sign=-1;
    if(p!=null)
      return p.getName(sign);
    else throw new ParticleNotFoundError(code);
  }

  public static int getParticleType(int code) {
    AParticle p=particles.get(new Integer(Math.abs(code)));

    if(p!=null)
      return p.getType();
    else throw new ParticleNotFoundError(code);
  }
  
  //Declare our own error type here - no special implementation
  public static class ParticleNotFoundError extends Error {
  
    //Only implement Error constructor with particle code
    ParticleNotFoundError(int code){
      super("Particle with code "+code+" not found in PDG table");
    }
  }

public static String getFileInCurrentUserThenAtlantisHome(String directory, String name)
{
    File user = new File(name);
    if (!user.canRead())
    {
        return AGlobals.instance().getHomeDirectory() + directory +
        		System.getProperty("file.separator") + name;
    }
    return name;
};
}
