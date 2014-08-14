package guitest;

import org.junit.Test;
import org.fest.swing.fixture.JComboBoxFixture;
import org.fest.swing.fixture.JTableCellFixture;

public class EnumeratorParameterTest extends AtlantisGuiTestCase {

	@Test
	public void test() {
		JTableCellFixture cell = guiFixture.findParameterCell("MuonDet","Track","Track Collections");
		cell.click();
		JComboBoxFixture comboBox = guiFixture.comboBox("Track Collections");
		String[] contents = comboBox.contents();
		comboBox.selectItem("All");
	}
}
