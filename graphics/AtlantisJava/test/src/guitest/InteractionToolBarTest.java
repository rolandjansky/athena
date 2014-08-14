package guitest;

import java.awt.event.KeyEvent;

import org.fest.swing.fixture.JComboBoxFixture;
import org.fest.swing.fixture.JTextComponentFixture;
import org.fest.swing.fixture.JToolBarFixture;
import org.junit.Test;

import static guitest.AAssert.*;

public class InteractionToolBarTest extends AtlantisGuiTestCase {
	@Test
	public void testInteractionToolBar() {
		JToolBarFixture toolBarFixture = guiFixture.findInteractionToolBar();

		toolBarFixture.toggleButton("ZMR").click();
		
		toolBarFixture.toggleButton("RubberBand").click();
		JComboBoxFixture rubberBandComboBox = toolBarFixture.comboBox();
		String[] listRubberBand = rubberBandComboBox.contents();
		String[] expectedRubberBand = {"RectangleYX","RotatedRectangle","Parallelogram",
				"Square","XSkew","YSkew","XSlice","YSlice"};
		assertArrayEqualsIgnoreOrder("Didn't find expected rubberband list",expectedRubberBand,listRubberBand);
		
		toolBarFixture.toggleButton("Pick").click();
		JComboBoxFixture pickComboBox = toolBarFixture.comboBox();
		String[] listPick = pickComboBox.contents();
		String[] expectedPick = {"Event Data","Detectors"};
		assertArrayEqualsIgnoreOrder("Didn't find expected pick list",expectedPick,listPick);
		
		toolBarFixture.toggleButton("SyncCursors").click();

		toolBarFixture.toggleButton("FishEye").click();
		JTextComponentFixture textBox = toolBarFixture.textBox();
		textBox.setText("");      // deleteText() does not work
		textBox.enterText("5.0"); // this does not press "enter", so...
		textBox.pressAndReleaseKeys(KeyEvent.VK_ENTER); // we do it here
		toolBarFixture.checkBox().click(); // turn off fish-eye
		toolBarFixture.checkBox().click(); // turn it on again

		toolBarFixture.toggleButton("Clock").click();
		toolBarFixture.checkBox().click(); // turn on clock transformation
		toolBarFixture.checkBox().click(); // turn it on again

		toolBarFixture.toggleButton("Scale").click();
	}
}
