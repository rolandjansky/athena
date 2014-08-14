package atlantis;

import static org.fest.swing.timing.Pause.pause;
import static org.fest.swing.timing.Timeout.timeout;
import static org.junit.Assert.*;

import org.fest.swing.timing.Condition;
import org.junit.Before;
import org.junit.Test;

/**
 * Test for bug#552, where a JiveXML file contains a "numTracks" element but
 * not a corresponding "tracks" element in the "RVx" section, as in some
 * of the events from the Hypatia masterclass.
 * 
 * @author waugh
 *
 */
public class NoTracksElementTest extends AtlantisHeadlessTestCase {
	@Before
	public void setUp() {
		eventFileName = "masterclassMuonEvents.zip";
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
		assertFalse("Error when constructing RVx",output.contains("Error when constructing RVx"));
	}
}
