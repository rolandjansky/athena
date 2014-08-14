package guitest;

import org.junit.Test;
import org.fest.swing.fixture.JTableFixture;

public class TabsTest extends AtlantisGuiTestCase {

	@Test
	public void testTabs() {
		JTableFixture table = guiFixture.findParametersTable("MuonDet","Segment");
		// Click on cell: does nothing but checks we have found required cell
		table.cell(table.cell("Segment Collections")).click();
		guiFixture.findParametersTable("Projection","YX");
		guiFixture.findParametersTable("Data","Data");
		guiFixture.findParametersTable("Cuts","Calo");
		guiFixture.findParametersTable("InDet","SpacePoint");
		guiFixture.findParametersTable("Calo","HEC");
		guiFixture.findParametersTable("MuonDet","MDT");
		guiFixture.findParametersTable("Objects","Muon");
		guiFixture.findParametersTable("Detector","Geo");
		guiFixture.findParametersTable("Appearance","Logo");
	}
}
