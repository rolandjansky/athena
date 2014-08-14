package atlantis.utils;

import java.util.Arrays;
import org.w3c.dom.Node;


/**
 * Class representing a range of possible values for an identifier field.
 *
 * @author Eric Jansen
 */
class AIdRange {
    
    /** Group the field belongs to, ranges of the same group should be merged. */
    private String group = null;
    
    /** Parent field, needed only for printing of labelled fields. */
    AIdField parent = null;
    
    /** A discrete range with values specified in this array. */
    private int[] values = null;
    
    /** A continuous range from min to max (if values == null). */
    private int minValue, maxValue;
    
    /**
     * Empty constructor, creates an empty range.
     */
    AIdRange() {
        this.values = new int[0];
    }

    /**
     * Continuous range constructor, contains all values &gt;= minValue &lt;= maxValue.
     * @param minValue first value in the range
     * @param maxValue last value in the range
     * @throws AAtlantisException 
     */
    AIdRange(int minValue, int maxValue) throws AAtlantisException {
        
        if (minValue > maxValue) {
            throw new AAtlantisException("Invalid range: " + minValue + " -> " + maxValue);
        }
        
        this.minValue = minValue;
        this.maxValue = maxValue;
    }
    
    /**
     * Discrete range constructor, contains the values given in values[].
     * @param values ordered array of values
     */
    AIdRange(int[] values) {
        this.values = values;
    }

    /**
     * XML constructor, initialize the range from a &lt;range&gt; element.
     * @param parent id field the values belong to
     * @param group group this field belongs to
     * @param node &lt;range&gt; XML element
     * @throws AAtlantisException 
     */
    AIdRange(AIdField parent, String group, Node node) throws AAtlantisException {
        
        String fieldName = node.getAttributes().getNamedItem("field").getNodeValue();
        this.parent = parent;
        this.group = group;

        Node minValueAttr = node.getAttributes().getNamedItem("minvalue");
        Node maxValueAttr = node.getAttributes().getNamedItem("maxvalue");

        if (minValueAttr != null && maxValueAttr != null) {

            minValue = parent.valueToInt(fieldName, minValueAttr.getNodeValue());
            maxValue = parent.valueToInt(fieldName, maxValueAttr.getNodeValue());
                        
            if (minValue > maxValue) {
                throw new AAtlantisException("Invalid range: " + minValue + " -> " + maxValue);
            }

        } else {
            Node valueAttr = node.getAttributes().getNamedItem("value");
            Node valuesAttr = node.getAttributes().getNamedItem("values");
            
            if (valueAttr != null) {
                values = new int[1];
                values[0] = parent.valueToInt(fieldName, valueAttr.getNodeValue());
            } else if (valuesAttr != null) {
                String[] list = valuesAttr.getNodeValue().split(" ");
                values = new int[list.length];
                for (int i=0; i<list.length; i++) {
                    values[i] = parent.valueToInt(fieldName, list[i]);
                }
            } else {
                values = new int[0];
            }
        }
    }
    
    /**
     * Get group name for this range.
     * @return group name
     */
    String group() {
        return group;
    }
    
    /**
     * Check if the range contains value.
     * @param value value to check
     * @return true if value is part of this range
     */
    boolean contains(int value) {
        if (values == null) {
            if (value >= minValue && value <= maxValue) {
                return true;
            } else {
                return false;
            }
        } else {
            for (int val : values) {
                if (val == value) {
                    return true;
                }
            }
            return false;
        }
    }
    
    /**
     * Check if the range fully contains another range.
     * @param range range to check
     * @return true if all values in the argument are in this range
     */
    boolean contains(AIdRange range) {
        if (range.values == null) {
            for (int value=range.minValue; value<=range.maxValue; value++) {
                if (!this.contains(value)) return false;
            }
        } else {
            for (int value : range.values) {
                if (!this.contains(value)) return false;                
            }
        }
        
        return true;
    }
    
    /**
     * Check if this range equals another.
     * @param obj other range
     * @return true if obj is an AIdRange object and its values match
     */
    @Override
    public boolean equals(Object obj) {

        if (!(obj instanceof AIdRange)) {
            return false;
        }

        AIdRange other = (AIdRange)obj;
        
        if (values == null && other.values == null) {

            // Continuous range, check boundaries
            if (minValue == other.minValue && maxValue == other.maxValue) {
                return true;
            } else {
                return false;
            }

        } else if (values == null) {
            
            // Continuous and discrete range
            if (other.values.length == maxValue - minValue + 1
                    && other.values[other.values.length-1] == maxValue
                    && other.values[0] == minValue) {
                return true;
            } else {
                return false;
            }
            
        } else if (other.values == null) {
            
            // Discrete and continuous range
            if (values.length == other.maxValue - other.minValue + 1
                    && values[values.length-1] == other.maxValue
                    && values[0] == other.minValue) {
                return true;
            } else {
                return false;
            }
            
        } else {

            // Discrete range, check length and individual entries
            if (values.length != other.values.length) {
                return false;
            } else {
                for (int i=0; i<values.length; i++) {
                    if (values[i] != other.values[i]) {
                        return false;
                    }
                }

                return true;
            }
        }
    }
    
    /**
     * Check if the range specifies any values or not.
     * @return true if the range is empty, false if not
     */
    public boolean isEmpty() {
        return values != null && values.length == 0;
    }

    /**
     * Hash code to facilitate lookups.
     * @return hash
     */
    @Override
    public int hashCode() {
        int hash = 7;
        hash = 89 * hash + Arrays.hashCode(this.values);
        hash = 89 * hash + this.minValue;
        hash = 89 * hash + this.maxValue;
        return hash;
    }
    
    /**
     * Merge the given range with the current one.
     * @param other range to be added to the current range
     * @return current range after new range has been added
     * @throws AAtlantisException 
     */
    AIdRange add(AIdRange other) throws AAtlantisException {

        if (values == null && other.values == null) {
            
            // Both are continuous ranges, find min and max values.
            minValue = Math.min(minValue, other.minValue);
            maxValue = Math.max(maxValue, other.maxValue);
            
        } else if (values == null || other.values == null) {

            // One is continous, the other is discrete.
            if (values == null) {

                if (other.values.length > 0) {
                    // If the discrete range is not empty, find min and max values.
                    minValue = Math.min(minValue, other.values[0]);
                    maxValue = Math.max(maxValue, other.values[other.values.length-1]);
                }
                
                // If we add an empty range, there is nothing to be done.

            } else {

                if (values.length > 0) {
                    // If the discrete range is not empty, find min and max values.
                    minValue = Math.min(values[0], other.minValue);
                    maxValue = Math.max(values[values.length-1], other.maxValue);
                    values = null;
                } else {
                    // if this is an empty range, copy min and max from the other.
                    minValue = other.minValue;
                    maxValue = other.maxValue;
                    values = null;
                }
            }

        } else {

            // Both ranges are discrete, count how many unique elements.
            int numValues = values.length;
            for (int value : other.values) {
                if (!this.contains(value)) {
                    numValues++;
                }
            }
            
            // Merge two arrays, skipping duplicates and keeping order.
            int[] newValues = new int[numValues];
            for (int i=0, j=0, k=0; i<numValues; i++) {
                if (j == values.length) {

                    newValues[i] = other.values[k++];                        

                } else if (k == other.values.length) {

                    newValues[i] = values[j++];

                } else {

                    if (this.values[j] == other.values[k]) {
                        newValues[i] = values[j++];
                        k++;
                    } else if (values[j] < other.values[k]) {
                        newValues[i] = values[j++];
                    } else {
                        newValues[i] = other.values[k++];                        
                    }
                }
            }
            
            values = newValues;
        }
        
        return this;
    }
    
    /**
     * Check the allowed values and return the number of bits needed to encode them.
     * @return number of bits
     */
    int bits() {
        
        // Cound how many values this field can have.
        int numValues;
        if (values != null) {
            numValues = values.length;
        } else {
            numValues = maxValue - minValue + 1;
        }
        
        if (numValues <= 2) {
            // A field takes at least 1 bit, so return 1.
            return 1;
        } else {
            // The number of bits required to store numValues different values.
            return (int)(Math.ceil(Math.log(numValues) / Math.log(2)));
        }
    }
    
    /**
     * Get the field value corresponding to the given range index. For example 
     * for a field that has range [ -2, 0, +2 ], get(0) will return -2.
     * @param index, decoded from compact identifier
     * @return field value
     * @throws AAtlantisException 
     */
    int get(int index) throws AAtlantisException {
        if (values != null) {
            if (index < values.length) {
                return values[index];
            } else {
                throw new AAtlantisException("Invalid field index: " + index);
            }
        } else {
            if (minValue + index <= maxValue) {
                return minValue + index;
            } else {
                throw new AAtlantisException("Invalid field value: " + index);
            }
        }
    }

    /**
     * Convert the range into a convenient form for printing.
     * @return formatted string representing range
     */
    @Override
    public String toString() {
        StringBuilder s = new StringBuilder();
        s.append("[ ");

        if (values == null) {
            if (parent != null && parent.hasLabels()) {
                String minLabel = parent.getLabel(minValue);
                String maxLabel = parent.getLabel(maxValue);
                s.append(minLabel).append("=").append(minValue);
                s.append(" -> ").append(maxLabel).append("=").append(maxValue);
            } else {
                s.append(minValue).append(" -> ").append(maxValue).append(" ");
            }
        } else {
            for (int value : values) {
                if (parent != null && parent.hasLabels()) {
                    String label = parent.getLabel(value);
                    s.append(label).append("=").append(value).append(" ");
                } else {
                    s.append(value).append(" ");
                }
            }
        }
        s.append("]");
        return s.toString();
    }    
}