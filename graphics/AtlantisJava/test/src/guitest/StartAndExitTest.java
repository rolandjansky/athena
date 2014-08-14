package guitest;

import java.awt.Dialog;
import org.junit.Test;
import org.fest.swing.core.matcher.DialogMatcher;
import org.fest.swing.core.matcher.JButtonMatcher;
import org.fest.swing.fixture.DialogFixture;

public class StartAndExitTest extends AtlantisGuiTestCase {

	@Test
	public void atlantisStarts() {
		guiFixture.menuItemWithPath("File", "Exit").click();
		DialogMatcher matcherConfirmExit = DialogMatcher.withTitle("Exit Atlantis");
		Dialog dialogConfirmExit = finder.find(matcherConfirmExit);
		DialogFixture fixtureDialogConfirmExit = new DialogFixture(robot,dialogConfirmExit);
		fixtureDialogConfirmExit.button(JButtonMatcher.withText("Yes")).click();
	}
}
