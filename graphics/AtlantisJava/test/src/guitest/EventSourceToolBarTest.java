package guitest;

import org.fest.swing.fixture.JToolBarFixture;
import org.junit.Test;

public class EventSourceToolBarTest extends AtlantisGuiTestCase {
	@Test
	public void testEventSourceToolBar() {
		JToolBarFixture toolBarFixture = guiFixture.findEventSourceToolBar();
		toolBarFixture.radioButton("Sequential mode").click();
		toolBarFixture.radioButton("Loop mode").click();
		toolBarFixture.radioButton("Random mode").click();
	}
}
