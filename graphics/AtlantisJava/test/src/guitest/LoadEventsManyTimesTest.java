package guitest;

import java.io.File;
import org.junit.Test;
import org.fest.swing.fixture.JButtonFixture;
import org.fest.swing.fixture.JToolBarFixture;

public class LoadEventsManyTimesTest extends AtlantisGuiTestCase {

	private static final int numEvents = 9;
	private static final int numRepeats = 3;
	
	@Test(timeout=45000)
	public void readEvents() {
		String eventsDirectory = System.getProperty("atlantis.test.events");
		guiFixture.openEventFileAndCheck(new File(eventsDirectory,"masterclassMuonEvents.zip"));
				JToolBarFixture toolBarFixture = guiFixture.findEventSourceToolBar();
				JButtonFixture nextButton = toolBarFixture.button("nextButton");
				JButtonFixture previousButton = toolBarFixture.button("previousButton");
  		for (int i=0; i<numRepeats; i++) {
			for (int iEvent=0; iEvent<(numEvents-1); iEvent++) {
				System.out.println("Click number "+iEvent);
				nextButton.click();
			}
			for (int iEvent=0; iEvent<(numEvents-1); iEvent++) {
				System.out.println("Click number "+iEvent);
				previousButton.click();
			}
		}
	}
}
