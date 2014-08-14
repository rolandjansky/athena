package atlantis.parameters;

import org.junit.Before;
import org.junit.Test;

import static org.junit.Assert.*;

/**
 * We don't test the methods that call APar to find out the current window,
 * which is not defined in unit tests.
 * 
 * @author waugh
 *
 */
public class AParameterDataTest {
	private static final int numWindows = 3;
	private static final double TOLERANCE = 0.001;
	
	private AParameterData paramData;
	
	@Before
	public void setUp() {
		APar.constructDummyForTesting();
		paramData = new AParameterData(numWindows);
	}

	@Test
	/**
	 * Check default values for all windows.
	 */
	public void constructDefault() {
		for (int iWindow=0; iWindow<numWindows; ++iWindow) {
			AParameterState state = paramData.getState(iWindow);
			assertValuesEqual("Default state is wrong.",0.0,false,"",state);
		}
	}
	
	@Test
	/**
	 * Copy values from window 1 to window 2.
	 */
	public void copyValues() {
		paramData.setValue(1, 1.0);
		paramData.setStatus(1, true);
		paramData.setOperator(1, "+");
		paramData.copy(1, 2);
		AParameterState state = paramData.getState(2);
		assertValuesEqual("Copied state is wrong.",1.0,true,"+",state);
	}
	
	@Test
	public void saveAndRestoreValues() {
		paramData.setValue(1, 1.0);
		paramData.setStatus(1, true);
		paramData.setOperator(1, "+");
		paramData.saveDefaults();
		paramData.setValue(1, 2.0);
		paramData.setStatus(1, false);
		paramData.setOperator(1, "-");
		AParameterState state = paramData.getState(1);
		assertValuesEqual("Set state is wrong.",2.0,false,"-",state);
		paramData.restoreDefaults();
		state = paramData.getState(1);
		assertValuesEqual("Restored state is wrong.",1.0,true,"+",state);
	}
	
	@Test
	public void testGlobalize() {
		paramData.setValue(1, 1.0);
		paramData.setStatus(1, true);
		paramData.setOperator(1, "+");
		paramData.globalize(1);
		for (int iWindow=0; iWindow<numWindows; ++iWindow) {
			AParameterState state = paramData.getState(iWindow);
			assertValuesEqual("State wrong after globalization.",1.0,true,"+",state);
		}
	}
	
	private static void assertValuesEqual(String message, double value, boolean status, String operator, AParameterState actual) {
		assertEquals(message+" Wrong value.",value,actual.value,TOLERANCE);
		assertEquals(message+" Wrong status.",status,actual.status);
		assertEquals(message+" Wrong operator.",operator,actual.operator);
	}
}
