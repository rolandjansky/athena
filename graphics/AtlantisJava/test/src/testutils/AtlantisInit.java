package testutils;

import atlantis.Atlantis;
import atlantis.globals.AGlobals;

/**
 * Do Atlantis initialization for tests: things that would be set up by main() in the
 * full application, and require access to protected Atlantis members.
 * 
 * @author waugh
 *
 */
public class AtlantisInit extends Atlantis {

	public static void init() {
	      String homeDirectory = Atlantis.getHomeDirectory();
	      AGlobals.instance().setHomeDirectory(homeDirectory);
	}

}
