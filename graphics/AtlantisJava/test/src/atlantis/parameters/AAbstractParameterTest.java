package atlantis.parameters;

import static org.junit.Assert.*;

import org.junit.Before;
import org.junit.Test;

import testutils.AtlantisInit;

import atlantis.canvas.ACanvas;

/**
 * Tests for AAbstractParameter.
 * @author waugh
 *
 */
public class AAbstractParameterTest {
	/** Arbitrary tolerance for floating-point comparisons */
	private static final double TOLERANCE = 0.00001;

	private AAbstractParameter parameter;

	@Before
	public void setUp() throws Exception {
		AtlantisInit.init();
		APar.constructDummyForTesting();
		ACanvas.constructDummyForTesting();
		parameter = new AAbstractParameter("name", "screen name", null, 0, 0, null, null, false, false, false, 0, 0, 0) {
			{wCount = 1; data = new AParameterData(wCount);}
			public void setD(double v) {}
			public void setI(int v) {}
			public void refresh() {}
		};
	}

	@Test
	public void testAAbstractParameter() {
		assertNotNull(parameter);
	}

	@Test
	public void testSaveAndRestoreDefaults() {
		// Access member variables directly since set and other methods rely on
		// information about existing windows. Rely on uninitialized
		// APar.currentIndex being zero.

		// Set value, status etc.
		parameter.valueType = AAbstractParameter.FLOAT;
		parameter.data.setValue(1.0);
		parameter.data.setStatus(false);
		parameter.data.setOperator("");
		parameter.range = AAbstractParameter.resolvePossibleValues("0:10", AAbstractParameter.FLOAT);
		parameter._setD(1.0);
		parameter.hasStatus = true;
		parameter.setStatus(true);
		parameter._setOperator("A"); // no restriction on value of operator?
		parameter.setScope(AAbstractParameter.LOCAL);
		// Save current settings as defaults then change them
		parameter.saveDefaults();
		parameter._setD(2.0);
		parameter.setStatus(false);
		parameter._setOperator("B");
		parameter.setScope(AAbstractParameter.GLOBAL);
		// Make sure the current settings have changed
		double value = parameter.getD();
		boolean status = parameter.getStatus();
		String operator = parameter.getOperator();
		int scope = parameter.getScope();
		assertEquals("parameter value not changed from default",2.0,value,TOLERANCE);
		assertEquals("parameter status not changed from default",false,status);
		assertEquals("parameter operator not changed from default","B",operator);
		assertEquals("parameter scope not changed from default",AAbstractParameter.GLOBAL,scope);
		// Then restore defaults and verify the original settings are back
		parameter.restoreDefaults();
		double restoredValue = parameter.getD();
		boolean restoredStatus = parameter.getStatus();
		String restoredOperator = parameter.getOperator();
		int restoredScope = parameter.getScope();
		assertEquals("parameter value not set to default",1.0,restoredValue,TOLERANCE);
		assertEquals("parameter status not set to default",true,restoredStatus);
		assertEquals("parameter operator not set to default","A",restoredOperator);
		assertEquals("parameter scope not set to default",AAbstractParameter.LOCAL,restoredScope);
	}
	
	@Test
	public void testPossibleValuesFloat() {
		parameter.valueType = AAbstractParameter.FLOAT;
		String possibleValues = "1.0:2.0,3.0:4.5";
		parameter.range = AAbstractParameter.resolvePossibleValues(possibleValues, AAbstractParameter.FLOAT);
		assertValidValue(parameter,possibleValues,1.2);
		assertValidValue(parameter,possibleValues,4.4);
		assertInvalidValue(parameter,possibleValues,0.9);
		assertInvalidValue(parameter,possibleValues,2.1);
		assertInvalidValue(parameter,possibleValues,9.9);
		assertInvalidValue(parameter,possibleValues,-1.2);
	}
	
	@Test
	public void testPossibleValuesInt() {
		parameter.valueType = AAbstractParameter.INT;
		String possibleValues = "1,2,4";
		parameter.range = AAbstractParameter.resolvePossibleValues(possibleValues, AAbstractParameter.FLOAT);
		assertValidValue(parameter,possibleValues,1);
		assertValidValue(parameter,possibleValues,2);
		assertValidValue(parameter,possibleValues,4);
		assertInvalidValue(parameter,possibleValues,3);
		assertInvalidValue(parameter,possibleValues,-1);
	}

	private static void assertValidValue(AAbstractParameter parameter, String possibleValues, double value) {
		boolean validated = parameter.validateValue(value);
		assertTrue(String.format("Value %f incorrectly fails validation with possible values %s",value,possibleValues),validated);
	}

	private static void assertInvalidValue(AAbstractParameter parameter, String possibleValues, double value) {
		boolean validated = parameter.validateValue(value);
		assertFalse(String.format("Value %f incorrectly passes validation with possible values %s",value,possibleValues),validated);
	}

}
