package atlantis;


import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

import testutils.TeeOutputStream;
import testutils.TestUtils;

public class AtlantisHeadlessTestCase {
	ByteArrayOutputStream stdOutCopy = new ByteArrayOutputStream();
	protected String eventFileName = null; // Event file to give on the command line

	/** Start Atlantis in headless mode.
	 */
	protected void startHeadlessMode() {
		// Redirect standard output to ByteArrayOutputStream as well as normal System.out
		TeeOutputStream teeOut = new TeeOutputStream(System.out,stdOutCopy);
		System.setOut(new PrintStream(teeOut));
		
		String atlantisHomeDir = Atlantis.getHomeDirectory();
		String testEventsDir = atlantisHomeDir+"/test/events";
		TestUtils.setPropertyIfNotSet("atlantis.test.events",testEventsDir);

		System.setProperty("java.awt.headless", "true");
		String[] args = {"--debug","DEBUG"};
		if (eventFileName!=null) {
			String[] newArgs = new String[args.length+1];
			for (int i=0; i<args.length; ++i) {
				newArgs[i] = args[i];
			}
			String eventsDirectory = System.getProperty("atlantis.test.events");
			newArgs[args.length] = eventsDirectory+"/"+eventFileName;
			args = newArgs;
		}
		Atlantis.main(args);
	}
}
