package guitest;

import static org.junit.Assert.*;

import java.awt.Dialog;
import java.io.File;

import org.fest.swing.core.matcher.DialogMatcher;
import org.fest.swing.exception.ComponentLookupException;
import org.junit.Test;

public class LoadEventWithValidRecVertexDataTest extends AtlantisGuiTestCase {
	@Test
	public void loadEvent() {
		String output = stdOutCopy.toString();
		assertTrue("Output does not contain \"Atlantis Ready\"",output.contains("Atlantis Ready"));
		String eventsDirectory = System.getProperty("atlantis.test.events");
		guiFixture.openEventFile(new File(eventsDirectory,"rvxEvent.xml"));
		try {
			Dialog dialogInvalid = finder.find(DialogMatcher.withTitle("Invalid source"));
			fail("Failed to open event: found invalid source dialog");
		}
		catch (ComponentLookupException e) {
			// OK
		}
		output = stdOutCopy.toString();
		assertFalse("Inconsistent data incorrectly detected",output.contains("RVx: numbers of tracks are inconsistent."));
	}
}
