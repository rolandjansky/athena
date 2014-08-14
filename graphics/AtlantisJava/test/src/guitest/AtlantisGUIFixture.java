package guitest;

import java.awt.Dialog;
import java.io.File;

import org.fest.swing.core.BasicComponentFinder;
import org.fest.swing.core.ComponentFinder;
import org.fest.swing.core.ComponentFoundCondition;
import org.fest.swing.core.Robot;
import org.fest.swing.core.matcher.DialogMatcher;
import org.fest.swing.core.matcher.FrameMatcher;
import org.fest.swing.core.matcher.JButtonMatcher;
import org.fest.swing.data.TableCell;
import org.fest.swing.data.TableCellByColumnId;
import org.fest.swing.exception.ComponentLookupException;
import org.fest.swing.fixture.DialogFixture;
import org.fest.swing.fixture.FrameFixture;
import org.fest.swing.fixture.JButtonFixture;
import org.fest.swing.fixture.JFileChooserFixture;
import org.fest.swing.fixture.JTabbedPaneFixture;
import org.fest.swing.fixture.JTableCellFixture;
import org.fest.swing.fixture.JTableFixture;
import org.fest.swing.fixture.JToolBarFixture;
import org.fest.swing.timing.Pause;

import static org.junit.Assert.*;

import atlantis.gui.AGUI;

/**
 * Fixture for testing the Atlantis GUI (class AGUI).
 * @author waugh
 *
 */
public class AtlantisGUIFixture extends FrameFixture {
	private Robot robot;
	private ComponentFinder finder;
	private FrameFixture guiFixture;

	/**
	 * Construct and return AtlantisGUIFixture using specified robot.
	 * @param robot the robot to use
	 * @return fixture for the Atlantis GUI
	 */
	public static AtlantisGUIFixture getFixture(Robot robot) {
		ComponentFinder finder = BasicComponentFinder.finderWithCurrentAwtHierarchy();
		FrameMatcher matcherGuiTitle = FrameMatcher.withTitle("Atlantis GUI");
		ComponentFoundCondition guiFound = new ComponentFoundCondition("Atlantis GUI found",
				finder, matcherGuiTitle);
		Pause.pause(guiFound,30000);
		AGUI gui = (AGUI) guiFound.found();
		return new AtlantisGUIFixture(robot,gui);
	}

	/**
	 * Construct AtlantisGUIFixture.
	 * @param robot
	 */
	public AtlantisGUIFixture(Robot robot, AGUI gui) {
		super(robot,gui);
		this.robot = robot;
		this.finder = BasicComponentFinder.finderWithCurrentAwtHierarchy();
		this.guiFixture = new FrameFixture(robot,gui);
	}

	/** Find AParametersTable for given supergroup and group */
	protected JTableFixture findParametersTable(String superGroup, String group) {
		JTabbedPaneFixture tabs = guiFixture.tabbedPane("parameterGroups");
		tabs.selectTab(superGroup);
		JTabbedPaneFixture subTabs = guiFixture.tabbedPane(superGroup);
		subTabs.selectTab(group);
		JTableFixture parametersTable = guiFixture.table(group);
		return parametersTable;
	}

	/**
	 * Find table cell containing given parameter.
	 * @param superGroup
	 * @param group
	 * @param parameter
	 * @return
	 */
	protected JTableCellFixture findParameterCell(String superGroup, String group, String parameter) {
		JTableFixture table = findParametersTable(superGroup,group);
		TableCell cellName = table.cell(parameter);
		int row = cellName.row; // Row containing cell with specified name
		JTableCellFixture cellValue = table.cell(TableCellByColumnId.row(row).columnId("Value"));
		return cellValue;
	}

	/** Find button in GUI with given text */
	protected JButtonFixture findGuiButton(String text) {
		JButtonMatcher matcher = JButtonMatcher.withText(text);
		return guiFixture.button(matcher);
	}

	/**
	 * Open file using GUI
	 * @param file event file
	 */
	protected void openEventFile(File file) {
		guiFixture.menuItemWithPath("File", "Read Event Locally").click();
		// Sometimes we have more than one "Open" dialog, but should be only one showing
		Dialog dialogOpen = finder.find(DialogMatcher.withTitle("Open").andShowing());
		DialogFixture fixtureDialogOpen = new DialogFixture(robot,dialogOpen);
		JFileChooserFixture fileChooser = fixtureDialogOpen.fileChooser();
		fileChooser.selectFile(file);
		fileChooser.approve();
	}

	/**
	 * Open file using GUI and fail if there is a problem
	 * @param file event file
	 */
	protected void openEventFileAndCheck(File file) {
		openEventFile(file);
		try {
			Dialog dialogInvalid = finder.find(DialogMatcher.withTitle("Invalid source"));
			fail("Failed to open event: found invalid source dialog");
		}
		catch (ComponentLookupException e) {
			// OK: no warning dialog!
		}
	}
	
	/**
	 * Get a fixture for the event source tool bar.
	 * @return
	 */
	public JToolBarFixture findEventSourceToolBar() {
		return new JToolBarFixture(robot,"Atlantis event source toolbar");
	}

	/**
	 * Get a fixture for the interactions tool bar.
	 * @return
	 */
	public JToolBarFixture findInteractionToolBar() {
		return new JToolBarFixture(robot,"Atlantis interaction toolbar");
	}
}
