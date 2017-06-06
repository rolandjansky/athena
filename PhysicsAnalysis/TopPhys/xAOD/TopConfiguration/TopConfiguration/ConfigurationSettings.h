/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef CONFIGURATIONSETTINGS_H_
#define CONFIGURATIONSETTINGS_H_

#include <vector>
#include <string>
#include <map>

#include "SelectionConfigurationData.h"
#include "StringData.h"

namespace top {

/**
 * @brief Hold the configuration information for the whole run.  A singleton,
 * so only one of these exists.  It needs populating with a call to loadFromFile
 * early on.  Otherwise it'll be full of rubbish and will refuse to run properly
 */
class ConfigurationSettings {
public:
    /**
     * @brief Design patterns 101.  A singleton getter function.
     */
    static ConfigurationSettings* get();

    /**
     * @brief The user wants to know the value for the specified key
     *
     * @param key The string key used to identify this variable.
     *
     * @return The setting for the given key.
     */
    const std::string& value(const std::string& key) const;

    /**
     * @brief The user wants to know the boolean value for the specified key
     *
     * @param key The string key used to identify this variable.
     * @param value On success, the setting for the given key (output).
     *
     * @return Indicates success (i.e. the value was not empty).
     *
     * Note: an attempt to retrieve a key with a non-boolean values raises an exception.
     */
    bool retrieve(std::string const & key, bool & value) const;

    /**
     * @brief Have the configuration settings been read from a file?
     *
     * @return True if configured (call to loadFromFile worked) otherwise false.
     */
    bool configured() const;

    /**
     * @brief Read the settings from a text file given by.
     *
     * @param filename The filename to read from.
     */
    void loadFromFile(const std::string& filename);

    /**
     * @brief All the user input that is stored as strings.
     *
     * @return a hased-map of the string name to the String data (description etc)
     */
    const std::map<std::string, StringData>& stringData() const;

    /**
     * @brief Get all the data needed to configure the selections.
     *
     * @return The selections in a vector.
     */
    const std::vector<SelectionConfigurationData> selections() const;

    /**
     * @brief Add a variable to the configuration settings.
     *
     * @param name The key used to refer to this variable.
     * @param message A human readable explanation
     */
  void registerParameter(const std::string& name, const std::string& message, const std::string& default_val="");

private:
    ///True if the class has read the settings from a file or something
    bool m_configured;

    ///Singletons: Good or bad, you decide. // John says they are bad
    static ConfigurationSettings* m_instance;

    ///Hold the configuration data
    std::map<std::string, StringData> strings_;

    ///Data used when configuring the selections
    std::vector<SelectionConfigurationData> m_selections;

    ///private constructor
    ConfigurationSettings();

    ///Here but not implemented, resulting in a happy compile error if you try and use them
    ConfigurationSettings(const ConfigurationSettings&);

    ///Here but not implemented, resulting in a happy compile error if you try and use them
    ConfigurationSettings operator=(const ConfigurationSettings&);
    
};

}

std::ostream& operator<<(std::ostream& os, const top::ConfigurationSettings& settings);
std::ostream& operator<<(std::ostream& os, const top::SelectionConfigurationData& data);


#endif
