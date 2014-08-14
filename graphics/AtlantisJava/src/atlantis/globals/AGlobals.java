/**
 * 
 */
package atlantis.globals;

import java.awt.Frame;
import java.awt.GraphicsEnvironment;


/**
 * Provides access to Atlantis global variables in a slightly less bad way than
 * using the main atlantis.Atlantis class.
 * 
 * @author waugh
 *
 */
public class AGlobals {
	/** The singleton instance. No point in lazy instantiation. */
	private static AGlobals globals = new AGlobals();
	
	/** Get the singleton instance */
	public static AGlobals instance() {return globals;}

	/** Prevent instantiation except through singleton accessor. */
	private AGlobals() {}
	
	/** Normally the directory that Atlantis was loaded from. */
	private String homeDirectory;

    /** The version of AtlantisJava from Svn. */
	private String version;

	/** Flag for displaying HLTAutoKey data. */
	private boolean showHLTAutoKeys = false;

	/** Flag for using simple output, used by Minerva. */
	private int SIMPLE_OUTPUT = 0;
	
    /** This is the URL that is used as default for "Read events from URL". */
    private String LIVE_POINT_1_EVENTS_URL;
    
    /** The path to the user's configuration file. */
    private String userConfigFile;
    
    /** Whether to use OpenGL. */
    private boolean useOpenGL;
    
    /** Whether we are in demo mode. */
    private boolean demoMode;
    
    /** Component representing the GUI, if any. */
    private Frame guiFrame;
    
	/**
	 * Set the Atlantis home directory, normally the directory that Atlantis was loaded from.
	 * 
	 * @param homeDirectory the Atlantis home directory
	 */
	public void setHomeDirectory(String homeDirectory) {
		this.homeDirectory = homeDirectory;
	}

	/**
	 * Get the Atlantis home directory, normally the directory that Atlantis was loaded from.
	 * 
	 * @return the Atlantis home directory
	 */
	public String getHomeDirectory() {
		return homeDirectory;
	}

	/**
	 * Get AtlantisJava version.
	 * @return the Svn version, set in Atlantis.main()
	 */
	public String getVersion() {
		return version;
	}

	/**
	 * Set AtlantisJava version, called from Atlantis.main().
	 * @param version the version to set
	 */
	public void setVersion(String version) {
		this.version = version;
	}

	/**
	 * Whether to show data with storeGateKey containing "HLTAutoKey".
	 *
	 * @return true to show, false to ignore
	 */
	public boolean showHLTAutoKeys() {
		return showHLTAutoKeys;
	}

	/**
	 * Whether to show data with storeGateKey containing "HLTAutoKey".
	 *
	 * @param showHLTAutoKeys true to show, false to ignore
	 */
	public void setHLTAutoKeys(boolean showHLTAutoKeys) {
		this.showHLTAutoKeys = showHLTAutoKeys;
	}

	/**
	 * @return the sIMPLE_OUTPUT
	 */
	public int getSimpleOutput() {
		return SIMPLE_OUTPUT;
	}

	/**
	 * @param sIMPLE_OUTPUT the sIMPLE_OUTPUT to set
	 */
	public void setSimpleOutput(int sIMPLE_OUTPUT) {
		SIMPLE_OUTPUT = sIMPLE_OUTPUT;
	}

	/**
	 * @return the URL for the Point 1 live events server
	 */
	public String getLivePoint1EventsURL() {
		return LIVE_POINT_1_EVENTS_URL;
	}

	/**
	 * @param livePoint1EventsURL the URL for the Point 1 live events server
	 */
	public void setLivePoint1EventsURL(String livePoint1EventsURL) {
		LIVE_POINT_1_EVENTS_URL = livePoint1EventsURL;
	}

	/**
	 * @param userConfigFilePath the path to the user's configuration file
	 */
	public void setUserConfigFile(String userConfigFilePath) {
		this.userConfigFile = userConfigFilePath;
	}
	
	/**
	 * @return the path to the user's configuration file
	 */
	public String getUserConfigFile() {
		return this.userConfigFile;
	}

	/**
	 * @return true if we should be using OpenGL
	 */
	public boolean getUseOpenGL() {
		return useOpenGL;
	}

	/**
	 * @param useOpenGL true to use OpenGL, false otherwise
	 */
	public void setUseOpenGL(boolean useOpenGL) {
		this.useOpenGL = useOpenGL;
	}

	/**
	 * Check whether we are in headless mode
	 * @return true if headless (no graphics available)
	 */
	public static boolean isAtlantisHeadless() {
	    return GraphicsEnvironment.isHeadless();
	}

	/**
	 * Whether the demo mode loop is running.
	 * @return true if in demo mode
	 */
	public boolean isDemoMode() {
		return demoMode;
	}

	/**
	 * Signal whether the demo loop is running.
	 * @param demoMode true when starting loop, false when stopping
	 */
	public void setDemoMode(boolean demoMode) {
		this.demoMode = demoMode;
	}

	/**
	 * For use as parent of dialogues, avoiding dependency on AGUI class itself.
	 * @return the GUI frame
	 */
	public Frame getGuiFrame() {
		return guiFrame;
	}

	/**
	 * Set the GUI frame, called from AGUI when the instance is created.
	 * @param guiFrame the GUI frame
	 */
	public void setGuiFrame(Frame guiFrame) {
		this.guiFrame = guiFrame;
	}

}
