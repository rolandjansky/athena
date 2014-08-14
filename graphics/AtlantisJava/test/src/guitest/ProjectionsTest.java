package guitest;

import org.fest.swing.fixture.JPanelFixture;
import org.junit.Test;

import static atlantis.utils.AMath.*; // for Greek letters

public class ProjectionsTest extends AtlantisGuiTestCase {
	@Test
	public void testWindowControls() {
		JPanelFixture windowControls = guiFixture.panel("windowControl");
		windowControls.label("W").click();
		guiFixture.findParametersTable("Projection","YX").click();
		guiFixture.findParametersTable("Projection",PHI+ETA).click();
		guiFixture.findParametersTable("Projection",RHO+"Z").click();
		guiFixture.findParametersTable("Projection",PHI+RHO).click();
		guiFixture.findParametersTable("Projection",PHI+"Z").click();
		guiFixture.findParametersTable("Projection","X'Z").click();
		guiFixture.findParametersTable("Projection","Y'Z").click();
		guiFixture.findParametersTable("Projection","3DBox").click();
		guiFixture.findParametersTable("Projection","LegoPlot").click();
		guiFixture.findParametersTable("Projection","Event Info").click();
	}
}
