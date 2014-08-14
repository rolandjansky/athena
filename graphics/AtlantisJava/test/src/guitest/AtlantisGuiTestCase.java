package guitest;

import java.awt.Frame;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

import org.fest.swing.core.BasicComponentFinder;
import org.fest.swing.core.BasicRobot;
import org.fest.swing.core.ComponentFinder;
import org.fest.swing.core.Robot;
import org.fest.swing.core.matcher.FrameMatcher;
import org.fest.swing.edt.FailOnThreadViolationRepaintManager;
import org.fest.swing.fixture.FrameFixture;
import org.fest.swing.security.NoExitSecurityManagerInstaller;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;

import testutils.AtlantisInit;
import testutils.TeeOutputStream;
import testutils.TestUtils;

import atlantis.Atlantis;
import atlantis.globals.AGlobals;

public class AtlantisGuiTestCase {
	protected Robot robot;
	protected ComponentFinder finder;
	protected FrameFixture canvasFixture;
	protected AtlantisGUIFixture guiFixture;
	ByteArrayOutputStream stdOutCopy = new ByteArrayOutputStream();
	private static NoExitSecurityManagerInstaller noExitSecurityManagerInstaller;

	/** Time [ms] to pause at end of test for easier viewing */
	protected long pauseLength = 0;

	/**
	 * Install NoExitSecurityManager because JUnit gets upset if the forked JVM exits
	 */
	@BeforeClass
	public static void setUpOnce() {
		noExitSecurityManagerInstaller = NoExitSecurityManagerInstaller.installNoExitSecurityManager();
	}

	@AfterClass
	public static void tearDownOnce() {
		noExitSecurityManagerInstaller.uninstall();
	}

	@Before
	public void setUp() {
		startAtlantis();
	}
	
	protected void startAtlantis() {
		AtlantisInit.init();
		String atlantisHomeDir=AGlobals.instance().getHomeDirectory();
		String testEventsDir = atlantisHomeDir+"/test/events";
		TestUtils.setPropertyIfNotSet("atlantis.test.events",testEventsDir);
		
		// Omit this for now since Atlantis main does not do set-up on EDT
		//		FailOnThreadViolationRepaintManager.install();

		finder = BasicComponentFinder.finderWithCurrentAwtHierarchy();
		robot = BasicRobot.robotWithCurrentAwtHierarchy();

		// Redirect standard output to ByteArrayOutputStream as well as normal System.out
		TeeOutputStream teeOut = new TeeOutputStream(System.out,stdOutCopy);
		System.setOut(new PrintStream(teeOut));

		// Launch Atlantis application
		String[] args = {"--debug","DEBUG"};
		Atlantis.main(args);
		// Don't use ACrashReporter to handle uncaught exceptions
		Thread.setDefaultUncaughtExceptionHandler(null);

		// Get fixtures for the GUI and Canvas
		guiFixture = AtlantisGUIFixture.getFixture(robot);
		FrameMatcher matcherCanvasTitle = FrameMatcher.withTitle("Atlantis Canvas");
		// Cannot cast to ACanvas because the ACanvas is actually a JPanel that creates its own
		// parent JFrame.
		Frame canvas = finder.find(matcherCanvasTitle);
		canvasFixture = new FrameFixture(robot,canvas);
	}
	
	@After
	public void tearDown() {
		pause(pauseLength);
	}

	/** Pause for a second so can see end result on screen */
	protected static void pause(long duration) {
		try {
			Thread.sleep(duration);
		} catch (InterruptedException e) {
			// allow to return if interrupted
		}
	}
}
