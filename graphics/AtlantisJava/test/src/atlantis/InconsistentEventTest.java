package atlantis;

import static org.fest.swing.timing.Pause.pause;
import static org.fest.swing.timing.Timeout.timeout;
import static org.junit.Assert.*;

import org.fest.swing.timing.Condition;
import org.junit.Before;
import org.junit.Test;

/**
 * Test for bug#556, where a JiveXML file from Atlas release 17 causes an error
 * on startup when the file is given on the command line, though not when it is
 * opened after Atlantis has started.
 * 
 * @author waugh
 *
 */
public class InconsistentEventTest extends AtlantisHeadlessTestCase {
	@Before
	public void setUp() {
		eventFileName = "rel17vtxEvent.xml";
		System.setProperty("java.awt.headless", "true");
		startHeadlessMode();
	}
	
	@Test
	public void loadEvent() {
		pause(new Condition("output to contain \"Atlantis Ready\"") {
			public boolean test() {return stdOutCopy.toString().contains("Atlantis Ready");}
		},timeout(10000));
		String output = stdOutCopy.toString();
		assertTrue("Could not find or parse event ",output.contains("Parsing event"));
		assertFalse("Error reading event",output.contains("Can not read events"));
	}
}
