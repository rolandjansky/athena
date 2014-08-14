package guitest;

import java.io.File;
import org.junit.Test;
import org.fest.swing.fixture.JComboBoxFixture;
import org.fest.swing.fixture.JTableCellFixture;
import static guitest.AAssert.*;

/**
 * Check list of muon track collections for Trac bug #507
 * Should be empty in empty event!
 * @author waugh
 *
 */
public class MuonTrackCollectionsEmptyTest extends AtlantisGuiTestCase {
	@Test
	public void loadEvents() {
		String eventsDirectory = System.getProperty("atlantis.test.events");
		guiFixture.openEventFileAndCheck(new File(eventsDirectory,"muonCollections.xml"));
		checkMuonTrackCollections("ConvertedMBoyTracks",
				"ConvertedMBoyMuonSpectroOnlyTracks",
				"All");
		guiFixture.openEventFileAndCheck(new File(eventsDirectory,"emptyEvent.xml"));
		checkMuonTrackCollections("None");
	}
	
	private void checkMuonTrackCollections(String... expected) {
		String[] contents = getMuonTrackCollections();
		assertArrayEqualsIgnoreOrder("Didn't find expected collections",expected,contents);
	}

	private String[] getMuonTrackCollections() {
		JTableCellFixture cell = guiFixture.findParameterCell("MuonDet","Track","Track Collections");
		cell.click();
		JComboBoxFixture comboBox = guiFixture.comboBox("Track Collections");
		String[] contents = comboBox.contents();
		return contents;
	}
}
