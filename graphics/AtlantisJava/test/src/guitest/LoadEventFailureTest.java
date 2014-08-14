package guitest;

import java.awt.Dialog;
import java.io.File;
import org.junit.Test;
import static org.junit.Assert.*;
import org.fest.swing.core.matcher.DialogMatcher;
import org.fest.swing.exception.ComponentLookupException;

public class LoadEventFailureTest extends AtlantisGuiTestCase {
	@Test
	public void loadEvent() {
		guiFixture.openEventFile(new File("/nosuchfile"));
		try {
			Dialog dialogInvalid = finder.find(DialogMatcher.withTitle("Invalid source"));
			// OK, correctly failed to open file
		}
		catch (ComponentLookupException e) {
			fail("Did not find invalid source dialog when opening non-existent event file");
		}
	}
}
