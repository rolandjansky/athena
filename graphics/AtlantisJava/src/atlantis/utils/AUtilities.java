package atlantis.utils;

import java.io.FileNotFoundException;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.File;
import java.util.Date;
import java.text.SimpleDateFormat;
import java.awt.Graphics2D;
import java.awt.Image;
import java.awt.RenderingHints;
import java.awt.event.MouseEvent;
import java.awt.image.BufferedImage;
import javax.swing.SwingUtilities;
import javax.swing.ImageIcon;


/**
 *
 * @author Gary Taylor, Zdenek Maxa
 */
public class AUtilities
{	
	public static ALogger logger = ALogger.getLogger(AUtilities.class);

    private static final String dateFormat = "yyyy-MM-dd-HH-mm-ss";


    private AUtilities()
    {
    } // AUtilities() -------------------------------------------------------


    /**
     * Allow Ctrl+click as alternative to right-click so can use single-button
     * mouse on the Mac computer
     */
    public static boolean isRightMouseButton(MouseEvent e)
    {
        return SwingUtilities.isRightMouseButton(e) ||
            e.isControlDown();

    } // isRightMouseButton() -----------------------------------------------

    /**
     * Returns string with current data and time formatted according to
     * dateFomat attribute (currently: yyyy-MM-dd-HH-mm-ss)
     * @return String
     */
    public static String getDateTimeString()
    {
        String r = null;
        Date d = new Date(System.currentTimeMillis());
        SimpleDateFormat sdf = new SimpleDateFormat(dateFormat);
        r = sdf.format(d);
        return r;

    } // getDateTimeString() ------------------------------------------------


    /**
     * Returns last directory and file name from fullPath which is assumed to be
     * some path to a file. This method assumes it is used when a file is retrieved
     * from a .jar file.
     * @param fullPath
     */
    private static String getLastDirectoryAndFileName(String fullPath)
    {
    	// if running on Windows, Atlantis.fileSep is '\', but accessing a .jar
    	// file uses '/' as file separator - so replace all '\' occurrences
    	// '/' (troubles when running via webstart (from jar file (/ as file
    	// separator) on windows (uses \ as file separator))
		logger.debug("Trying to extract last directory and filename, input: " + fullPath);
    	char[] charArray = fullPath.replaceAll("\\\\", "/").toCharArray();
		int sepOccurence = 0; // file separator (e.g. '/') occurrence counter
		int i;
		for(i = fullPath.length() - 1; i > 0; i--)
		{
			// compare only to '/', even on Windows '\' was already replaced
			// by '/'
			if(charArray[i] == '/')
			{
				sepOccurence++;
			}
			if(sepOccurence == 2)
			{
				// i variable now holds second file separator occurrence from end
				break;
			}
		}
		
		String relPath = null;
		
		if(sepOccurence == 0)
		{
		    logger.warn("No file separator found in the file path: " + fullPath);
		    relPath = fullPath;
		}
        else if(i == 0)
        {
            logger.warn("The path looks like relative path already, using: " +
                        fullPath);
            relPath = fullPath;
        }
		else
		{
		    relPath = new String(charArray).substring(i + 1);
		    logger.debug("Last directory and filename result: " + relPath);
		}
		
		return relPath;

    } // getLastDirectoryAndFileName() --------------------------------------
    
    
    
    /**
     * Reads either locally (full path in fileName) or from atlantis.jar. Using
     * this method for reading files makes Atlantis running either with runtime
     * distribution directories (e.g. img, geometry, etc) or from .jar file
     * (everything including runtime directories packed in a jar file)
     * transparent. Having everything in .jar files is a requirement for 
     * running as Java WebStart.
     * This method is used for files (text, xml) as InputStream.   
     * @param fileName - full path
     */    
    public static InputStream getFileAsStream(String fileName) throws AAtlantisException
    {
    	InputStream is = null;
    	
    	if(fileName == null || "".equals(fileName))
    	{
    		String m = "Input parameter fileName is null or empty";
    		logger.debug(m);
    		throw new AAtlantisException(m);
    	}
    	
    	try
    	{
    		logger.debug("Trying to access file: " + fileName + " ...");
    		is = new FileInputStream(fileName);
    		logger.debug("Reading " + fileName + " successful.");    		
    	}
    	catch(FileNotFoundException fnfe)
    	{
    		logger.warn(fileName + " not found, trying atlantis.jar ... ");
    		
    		// need to get file name and the last directory from the full path    		
    		String relPath = getLastDirectoryAndFileName(fileName);
    		    		    		
    		ClassLoader cl = AUtilities.class.getClassLoader();
    		is = cl.getResourceAsStream(relPath);
    		if(is == null)
    		{
    			String m = fileName + " doesn't exist, reading " + relPath + 
    			           " from atlantis.jar failed.";
    			logger.error(m);
    			throw new AAtlantisException(m);
    		}
    		logger.debug(fileName + " reading from atlantis.jar successful.");
    	}
    	
    	return is;
    	
    } // getFileAsStream() --------------------------------------------------
    
    
    
    /**
     * Reads either locally (full path in fileName) or from atlantis.jar. Using
     * this method for reading files makes Atlantis running either with runtime
     * distribution directories (e.g. img, geometry, etc) or from .jar file
     * (everything including runtime directories packed in a jar file)
     * transparent. Having everything in .jar files is a requirement for 
     * running as Java WebStart.
     * This method is used for reading images.   
     * @param fileName - full path
     */
    public static ImageIcon getFileAsImageIcon(String fileName)
    {
    	
    	ImageIcon ii = null;
    	
    	if(fileName == null || "".equals(fileName))
    	{
    		String m = "Input parameter fileName is null or empty, can't get ImageIcon";
    		logger.debug(m);
    		return null;
    	}
    	
    	File test = new File(fileName);
    	if(test.exists() && test.canRead())
    	{
    		ii = new ImageIcon(fileName);
    		logger.debug("Reading " + fileName + " successful.");
    	}
    	else
    	{
    		logger.debug(fileName + " not found, trying atlantis.jar ... ");
    		
    		// need to get file name and the last directory from the full path
    		String relPath = getLastDirectoryAndFileName(fileName);    		
        
    		ClassLoader cl = AUtilities.class.getClassLoader();
    		try {
        		ii = new ImageIcon(cl.getResource(relPath));
    			logger.debug(fileName + " reading from atlantis.jar successful.");
    		}
    		catch (Throwable e) {
    			String m = "Failed to read "+ fileName + " from " + relPath + 
 			           " in atlantis.jar";
    			logger.debug(m);
    		}
    	}
    	
    	return ii;
    	
    } // getFileAsImageIcon() -----------------------------------------------
    
    // Function used to rescale an Image and return an ImageIcon
	//
   public static ImageIcon scale(Image src, double scale, boolean antiAlias) {
        int w = (int)(scale*src.getWidth(null));
        int h = (int)(scale*src.getHeight(null));
        int type = BufferedImage.TYPE_INT_ARGB;
        BufferedImage dst = new BufferedImage(w, h, type);
        Graphics2D g2 = dst.createGraphics();
        if(antiAlias) // Render png High Quality when AntiAliasing selected
        {
        	g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_BICUBIC);
        	g2.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_QUALITY);
        }
        else // else render it for speed.
        {
        	g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION, RenderingHints.VALUE_INTERPOLATION_NEAREST_NEIGHBOR);
        	g2.setRenderingHint(RenderingHints.KEY_RENDERING, RenderingHints.VALUE_RENDER_SPEED);
        }
        g2.drawImage(src, 0, 0, w, h, null);
        g2.dispose();
        return new ImageIcon(dst);
    } // ------- scale()

} // class AUtilities =======================================================
