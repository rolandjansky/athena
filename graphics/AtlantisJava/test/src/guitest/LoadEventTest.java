package guitest;

import static org.junit.Assert.*;

import java.awt.Dialog;
import java.io.File;

import org.fest.swing.core.matcher.DialogMatcher;
import org.fest.swing.exception.ComponentLookupException;
import org.junit.Test;

public class LoadEventTest extends AtlantisGuiTestCase {
	@Test
	public void loadEvent() {
		String eventsDirectory = System.getProperty("atlantis.test.events");
		guiFixture.openEventFile(new File(eventsDirectory,"muonCollections.xml"));
		try {
			Dialog dialogInvalid = finder.find(DialogMatcher.withTitle("Invalid source"));
			fail("Failed to open event: found invalid source dialog");
		}
		catch (ComponentLookupException e) {
			// OK
		}
	}
}
