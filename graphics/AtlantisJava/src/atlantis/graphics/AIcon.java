package atlantis.graphics;

import java.awt.Frame;

import javax.swing.ImageIcon;

import atlantis.globals.AGlobals;
import atlantis.utils.AUtilities;

/**
 * 
 * @author waugh
 *
 */
public class AIcon {
	/** Set icon image for a frame to be the Atlantis icon. */
	public static void setIconImage(Frame f)
	{
	    String path = AGlobals.instance().getHomeDirectory() + "img" + 
	    		System.getProperty("file.separator") +
	    		"atlantis_icon.gif";
	    ImageIcon ii = AUtilities.getFileAsImageIcon(path);
	    f.setIconImage(ii.getImage());
	
	} // setIconImage() -----------------------------------------------------


}
