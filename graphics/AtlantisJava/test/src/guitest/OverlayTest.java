package guitest;

import org.junit.Test;
import org.fest.swing.data.TableCellByColumnId;
import org.fest.swing.fixture.JTableCellFixture;
import org.fest.swing.fixture.JTableFixture;

public class OverlayTest extends AtlantisGuiTestCase {

	@Test
	public void testOverlay() {
		JTableFixture table = guiFixture.findParametersTable("Appearance","Logo");
		int row = 0;
		JTableCellFixture check = table.cell(TableCellByColumnId.row(row).columnId("Name"));
		//check the box to show logo
		check.click();
		//change the x position
		JTableCellFixture logox = guiFixture.findParameterCell("Appearance","Logo", "Logo X");
		logox.enterValue("0.5\n");
		//resize
		JTableCellFixture size = guiFixture.findParameterCell("Appearance","Logo", "Logo Size");
		size.enterValue("0.7\n");
		size.click();
		//now uncheck the box
		check.click();
	}
}
