package guitest;

import java.awt.Dialog;

import org.fest.swing.core.matcher.DialogMatcher;
import org.fest.swing.core.matcher.JButtonMatcher;
import org.fest.swing.fixture.DialogFixture;
import org.fest.swing.fixture.JButtonFixture;
import org.fest.swing.fixture.JToolBarFixture;
import org.junit.Test;

public class ReadNextAndPreviousEventTest extends AtlantisGuiTestCase {

	@Test
	public void readNextThenPreviousEvent() {
		JToolBarFixture toolBarFixture = guiFixture.findEventSourceToolBar();
		JButtonFixture nextButton = toolBarFixture.button("nextButton");
		JButtonFixture previousButton = toolBarFixture.button("previousButton");
		nextButton.click();
		previousButton.click();
		guiFixture.menuItemWithPath("File", "Exit").click();
		DialogMatcher matcherConfirmExit = DialogMatcher.withTitle("Exit Atlantis");
		Dialog dialogConfirmExit = finder.find(matcherConfirmExit);
		DialogFixture fixtureDialogConfirmExit = new DialogFixture(robot,dialogConfirmExit);
	//	fixtureDialogConfirmExit.button(JButtonMatcher.withText("Yes")).click();
	}
}
