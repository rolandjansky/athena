package atlantis.config;

import java.io.File;
import java.io.FileOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import java.util.Iterator;

import atlantis.utils.AUtilities;
import atlantis.utils.AAtlantisException;
import atlantis.utils.ALogger;



/**
 * This class encapsulates a few static methods which ensure updating
 * of the user's customised Atlantis configuration (or special Atlantis
 * configurations) according to the base Atlantis configuration file
 * configuration/config.xml
 * 
 * Updating means synchronising special / customised configurations if
 * XML elements were added/removed from the base configuration file while
 * keeping unchanged specially defined user cuts, views, projections, etc.
 * 
 * Methods in this class are also called by an external configuration
 * synchronization tool, so methods should remain static, special treatment
 * of logger, etc.
 * 
 * @author
 * Zdenek Maxa - initial update mechanism, creating this extra class, refactoring
 * Qiang Lu - improved reliable updating mechanism as done in AConfig 
 *
 */
public final class AConfigUpdater
{
    // when AConfigUpdater is called from Atlantis code, logger would behave
    // as configured there. If AConfigUpdater is called externally, logger
    // should be initialised with default values from ALogger
    private static ALogger logger = ALogger.getLogger(AConfigUpdater.class);
    
    
    

    /**
     * Methods creates checksum file (name in fileNameChk) of a given file
     * (file name in fileName).
     */
    public static void createChecksumFile(String fileName, String fileNameChk)
    throws AAtlantisException
    {
        try
        {
            logger.debug("Creating checksum file " + fileNameChk +
                         " of a file " + fileName);
            byte[] chk = calculateChecksum(fileName);
            File fileChk = new File(fileNameChk);
            OutputStream os = new FileOutputStream(fileChk);
            os.write(chk);
            os.close();
        }
        catch(AAtlantisException ae)
        {
            String m = "Error while creating MD5 checksum file, reason: " +
                       ae.getMessage();
            throw new AAtlantisException(m);
        }
        catch(FileNotFoundException fnfe)
        {
            String m = "File not found exception: " + fnfe.getMessage();
            throw new AAtlantisException(m);
        }
        catch(IOException ioe)
        {
            String m = "I/O error while creating file " + fileNameChk;
            throw new AAtlantisException(m);
        }
        
    } // createChecksumFile() -----------------------------------------------

    
    
    /**
     * Calculates checksum of a file fileName. 
     */
    private static byte[] calculateChecksum(String fileName) 
    throws AAtlantisException
    {
        InputStream fis = null;
        try
        {
            logger.debug("Calculating checksum of file: " + fileName);
            fis = AUtilities.getFileAsStream(fileName);
        }
        catch(AAtlantisException ae)
        {
            throw ae;
        }

        MessageDigest complete = null;
        
        try
        {
            byte[] buffer = new byte[1024];
            complete = MessageDigest.getInstance("MD5");
            int numRead;
            do
            {
                numRead = fis.read(buffer);
                if (numRead > 0)
                {
                    complete.update(buffer, 0, numRead);
                }
            } while (numRead != -1);
            
            fis.close();
        }
        catch(IOException ioe)
        {
            throw new AAtlantisException("Error while reading data from file " +
                                         fileName);
        }
        catch(NoSuchAlgorithmException nsae)
        {
            throw new AAtlantisException(nsae.getMessage());
        }
        
        logger.debug("Checksum calculated.");
        return complete.digest();
        
    } // calculateChecksum() ------------------------------------------------
    
    

    /**
     * Method checks the checksum of input file fileName if its checksum matches
     * checksum stored in the checksum file fileNameChk.
     * Return true if matches, false otherwise.
     */
    public static boolean compareChecksums(String fileName, String fileNameChk)
    throws AAtlantisException
    {
        boolean result = false;
        try
        {
            // checksum of the input file
            byte[] inputFileChecksum = calculateChecksum(fileName);
            // checksum to read stored checksum from file fileNameChk
            // assume the lenght is the same as of the input data file
            byte[] storedChecksum = new byte[inputFileChecksum.length];
            File fileChk = new File(fileNameChk);
            InputStream is = new FileInputStream(fileChk);
            is.read(storedChecksum);

            // compare both checksums
            if(new String(storedChecksum).equals(new String(inputFileChecksum)))
            {
                result = true;
            }
            else
            {
                result = false;
            }

            is.close();
        }
        catch(Exception e)
        {
            String m = "Error while checking MD5 checksum file, reason: " +
                       e.getMessage();
            throw new AAtlantisException(m);
        }
        
        return result;
        
    } // compareChecksums() -------------------------------------------------
    
    

    /**  
     * @param distConfig - configuration instance from Atlantis distribution
     *                     (master copy)
     * @param userConfig - user's customised or special configuration which
     *                     gets updated according to distribution configuration
     */
    private static void updateConfiguration(AConfig distConfig, AConfig userConfig)
    {
        logger.debug("Updating customised configuration ...");
        Iterator distIt = distConfig.getIterator();
        while(distIt.hasNext())
        {
            // For each key node (keyValue is not empty) inside distConfig,
            // test if there is a same node in userConfig.
            AConfigNode testDistNode = (AConfigNode) distIt.next();
            if(testDistNode.getKeyValue() != null)
            {
                AConfigNode matchUserNode = userConfig.find(testDistNode);
                if(matchUserNode != null)
                {
                    // update testDistNode based on matchUserNode
                    updateNode(distConfig, testDistNode, matchUserNode);
                }
            }
        }
        
        logger.debug("Update finished.");
        
    } // updateConfiguration() ----------------------------------------------

    
    
    /**
     * Update a node from configuration.
     */
    private static void updateNode(AConfig distConfig,
                                   AConfigNode distNode, AConfigNode userNode)
    {
        String[] attStrArray = null;
        if("Canvas".equals(distNode.getNodeName()))
        {
            attStrArray = AConfig.CANVAS_ATT;
        }
        else if("UsedWindow".equals(distNode.getNodeName()))
        {
            attStrArray = AConfig.WINDOW_ATT;
        }
        else if("Layout".equals(distNode.getNodeName()))
        {
            attStrArray = AConfig.LAYOUT_ATT;
        }
        else
        {
            //check the parent Group values
            if(distNode.getParent().getNodeName().equals("Group")){
                attStrArray = AConfig.GROUP_ATT;
                //update the value
                for(int i = 0; i < attStrArray.length; ++i)
                {
                    String att = attStrArray[i];
                    String distValue = distNode.getParent().getAttValue(att);
                    String userValue = userNode.getParent().getAttValue(att);
                        
                    if(distValue != null && userValue != null && !distValue.equals(userValue))
                    {
                        if(distConfig.validate(distNode.getParent(), att, userValue))
                        {
                            distNode.getParent().setAttValue(att, userValue);
                        }
                    }
                }
            }
            
            //now update parameters inside the Group
            attStrArray = AConfig.PAR_ATT;
        }

        // update the value
        for(int i = 0; i < attStrArray.length; ++i)
        {
            String att = attStrArray[i];
            String distValue = distNode.getAttValue(att);
            String userValue = userNode.getAttValue(att);
            if(distValue != null && userValue != null && !distValue.equals(userValue))
            {
                if(distConfig.validate(distNode, att, userValue))
                {
                    distNode.setAttValue(att, userValue);
                }
            }
        }
        
    } // updateNode() -------------------------------------------------------

    

    private static void writeUpdatedConfigurationFile(AConfig distConfig,
                                                      AConfig userConfig,
                                                      String filename)
    throws Exception
    {
        FileOutputStream fo = null;
        fo = new FileOutputStream(filename);
        String updatedConfiguration = distConfig.convertToString(userConfig);
        fo.write(updatedConfiguration.getBytes());
        fo.close();
        
    } // writeUpdatedConfigurationFile() ------------------------------------
    
    
    
    /**
     * Updates customised configuration file to be consistent with the
     * distribution configuration file.
     * @param fileNameDist - distribution configuration file path
     * @param fileNameCustom - customised configuration file path
     * @throws AAException
     */
    public static void updateConfigurationFile(String fileNameDist,
                                               String fileNameCustom)
    throws AAtlantisException
    {

        logger.info("Updating configuration - master " +
                    "configuration: " + fileNameDist);
        logger.info("Updating configuration - customised (being updated) " +
                    "configuration: " + fileNameCustom);
                
        AConfig distConfig = null;
        AConfig customConfig = null;

        // read master (distribution) and customised configurations
        try
        {
            distConfig = new AConfig(fileNameDist, false); // false - is not customised
        }
        catch(Exception e)
        {
            // these however seem to be never thrown because of AXMLErrorHandler
            // in AConfig
            String m = "Could not read/parse distribution configuration " +
                       fileNameDist + " reason: " + e.getMessage();
            throw new AAtlantisException(m);
        }
        
        try
        {
            customConfig = new AConfig(fileNameCustom, true); // true - is customised
        }
        catch(Exception e)
        {
            // these however seem to be never thrown because of AXMLErrorHandler
            // in AConfig
            String m = "Could not read/parse customised configuration " +
                       fileNameCustom + " reason: " + e.getMessage();
            throw new AAtlantisException(m);
        }

       
        // actual update
        try
        {
            updateConfiguration(distConfig, customConfig);
        }
        catch(Throwable t)
        {
            String m = "Updating customised configuration failed, reason: " +
                       t.getMessage();
            logger.debug(m, t);
            throw new AAtlantisException(m);
        }
        
        
        // write updated data, first create backup of the source file
        
        logger.info("Creating backup of the customised configuration file ...");
        // file name to backup (rename) the file to
        String fileNameUserBackup = fileNameCustom + "-backup_on_" +
                                    AUtilities.getDateTimeString();
        File fileCustom = new File(fileNameCustom);
        File fileBackup = new File(fileNameUserBackup);
        boolean renameResult = fileCustom.renameTo(fileBackup);
        if(! renameResult)
        {
            String m = "Could not backup customised configuration " +
                       "file, updating configuration failed.";
            
            // if fileNameDist path starts with "//", then it is run from within
            // JAR archive via webstart. update process can't be performed in this
            // case - can't create a backup copy of the fileNameCustom (and can't
            // create an updated file when all is packed in a single JAR file either)
            if(fileNameDist.startsWith("//"))
            {
                m += " Running via webstart, update can't be perfomed in this mode.";
            }
            throw new AAtlantisException(m);
        }
        logger.info(fileNameCustom + " backup created: " + fileNameUserBackup);        
        
        
        try
        {
            writeUpdatedConfigurationFile(distConfig, customConfig,
                                          fileNameCustom);
        }
        catch(Throwable t)
        {
            String m = "Saving updated customised configuration into file " +
                       "failed, reason: " + t.getMessage();
            logger.debug(m, t);
            throw new AAtlantisException(m);
        }
        
        
        logger.info(fileNameCustom + " configuration was successfully updated.");
        
    } // updateConfigurationFile() ------------------------------------------

  
} // class AConfigUpdater ===================================================