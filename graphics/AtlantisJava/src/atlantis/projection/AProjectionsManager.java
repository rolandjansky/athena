package atlantis.projection;

import java.util.Enumeration;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.Set;
import java.util.Vector;

import atlantis.output.AExceptionHandler;
import atlantis.parameters.APar;
import atlantis.parameters.AParametersGroup;

/**
 * This manager keeps references to all projections.
 * 
 * The code keeping track of the list and states of interactions associated with each
 * projection in each window, and creating the corresponding pop-up menus, has been
 * moved to atlantis.gui.AInteractionToolBar and atlantis.interactions.AInteractionsByProjection.
 */
public class AProjectionsManager {
	
	// the set of projections (name : AProjection)
	private static Hashtable<String,AProjection> projections = new Hashtable<String, AProjection>();
	private static Set<String> projectionsAvailableInDemoMode = new HashSet<String>();
	private static Set<String> defaultProjectionsInDemoMode = new HashSet<String>();

	/**
	 * Creates projections using information from APar.
	 */
	public static void initialise() {
		Vector<String> projVector=APar.instance().getUIGroupNames(AParametersGroup.PROJECTION);
		
		for (String projName : projVector) {
			createProjection(projName);
		}
	}
	
	/**
	 * Used to request the projection with the given name
	 * @param projectionName The name of the projection
	 * @return The AProjection corespoinding to the projection name.
	 */
	public static AProjection getProjection(String projectionName) {
		return projections.get(projectionName);
	}
	
	/**
	 * Checks whether the given name is a valid projection name.
	 * @param name The name to be checked
	 * @return True/False
	 */
	public static boolean isValidProjection(String name) {
		return projections.containsKey(name);
	}
	
	/**
	 * Returns an array containing the names of all known projections
	 * @return The name array
	 */
	public static String[] getKnownProjectionNames() {
		String[] names=new String[projections.size()];
		
		Enumeration<String> myenum=projections.keys();
		
		for(int i=0; i<projections.size(); i++)
			names[i]=myenum.nextElement();
		
		return names;
	}

	/**
	 * Check whether the projection should be used in the demo dialog
	 * @return false if the projection should not be used or does not exist
	 */
	public static boolean useProjectionInDemoMode(String name){
		return projectionsAvailableInDemoMode.contains(name);
	}

	/**
	 * @return true if  the projection should be selected by default in the demo dialog
	 */
	public static boolean defaultProjectionInDemoMode(String name){
		return defaultProjectionsInDemoMode.contains(name);
	}

	/**
	 * Creates an instance of a given projection and pushes it into the
	 * <code>projections</code> Hashtable. The name shold be for example: YX if you
	 * want to create a instance of AProjectionYX.java class.
	 * @param name The name of the projection.
	 */
	private static void createProjection(String name) {
		try {
			
			AProjection p=(AProjection)Class.forName("atlantis.projection.AProjection"+name).newInstance();
			
			projections.put(name, p);
		} catch(Exception e) {
			AExceptionHandler.processException("Cannot create AProjection"+name, e);
		}
	}

	public static void makeProjectionAvailableInDemoMode(String projection) {
		projectionsAvailableInDemoMode.add(projection);
	}

	public static void makeProjectionDefaultInDemoMode(String name) {
		defaultProjectionsInDemoMode.add(name);
	}
}
