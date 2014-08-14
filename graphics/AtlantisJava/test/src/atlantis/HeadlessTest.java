package atlantis;

import org.fest.swing.timing.Condition;
import org.junit.Test;
import static org.fest.swing.timing.Pause.pause;
import static org.fest.swing.timing.Timeout.timeout;

public class HeadlessTest extends AtlantisHeadlessTestCase {

	@Test
	/** Start Atlantis in headless mode and make sure it doesn't crash part-way through.
	 *  Atlantis.initAtlantis catches all Exceptions so we check for "Atlantis Ready" in
	 *  the output console.
	 */
	public void testHeadlessMode() {
		startHeadlessMode();
		pause(new Condition("output to contain \"Atlantis Ready\"") {
			public boolean test() {return stdOutCopy.toString().contains("Atlantis Ready");}
		},timeout(10000));
	}
}
