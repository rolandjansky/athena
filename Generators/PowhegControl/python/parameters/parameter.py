# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

import collections


class Parameter(object):
    """! Class to store information about a single Powheg parameter.

    @author James Robinson  <james.robinson@cern.ch>
    """

    def __init__(self, keyword=None, name=None, value=None, description=None, frozen=False, hidden=False, needs_processing=False):
        """! Initialise a parameter object.

        @param keyword           Powheg keyword.
        @param name              User-visible name.
        @param value             Current value.
        @param description       Human-readable description string.
        @param frozen            True if this parameter cannot be changed.
        @param hidden            True if this parameter should be hidden from the user.
        @param needs_processing  True if this parameter needs further processing.
        """
        self.__keyword = keyword
        self.__name = name
        self.__description = description
        self.__value = value
        self.__default_value = None
        self.__frozen = frozen
        self.__hidden = hidden
        self.__needs_processing = needs_processing

    def __str__(self):
        """! String representation."""
        current_value = [self.value, "'{}'".format(self.value)][any(x in self.name for x in ("lhrwgt", "rwl_file"))]  # some parameters need to be in quotes
        pwg_cmd = "{} {}".format(self.keyword, self.__single_element(current_value))                                  # construct command for PowhegBox
        return "{:<40}! [ATLAS default: {}] {}".format(pwg_cmd, self.default_value, self.description)

    def ensure_default(self):
        """! Ensure that default value exists."""
        if self.__default_value is None:
            self.__default_value = self.value

    def __single_element(self, to_check):
        """! Return the first element if input is a list, or the input otherwise.

        @param to_check An iterable or non-iterable parameter
        """
        if isinstance(to_check, collections.Iterable) and not isinstance(to_check, str) and len(to_check) > 0:
            return to_check[0]
        return to_check

    def freeze(self):
        """! Freeze this parameter so that it cannot be changed."""
        self.__frozen = True

    @property
    def keyword(self):
        """! Retrieve the Powheg keyword."""
        return self.__keyword

    @property
    def name(self):
        """! Retrieve the user-visible name."""
        return self.__name

    @property
    def needs_processing(self):
        """! Indicator that this parameter needs further processing."""
        return self.__needs_processing

    @property
    def value(self):
        """! Retrieve the current value of this parameter."""
        return self.__value

    @property
    def default_value(self):
        """! Retrieve the default value of this parameter (if it exists)."""
        if self.__default_value is None:
            return self.value
        return self.__default_value

    @property
    def description(self):
        """! Retrieve the human-readable description string."""
        return self.__description

    @property
    def is_frozen(self):
        """! Boolean indicating whether this parameter can be changed."""
        return self.__frozen

    @property
    def is_hidden(self):
        """! Boolean indicating whether this parameter is visible to the user."""
        return self.__hidden

    @property
    def is_visible(self):
        """! Boolean indicating whether this parameter should be hidden from the user."""
        return not (self.__frozen or self.__hidden)

    @value.setter
    def value(self, value):
        """! Set the value of this parameter (if it is not frozen)."""
        if self.__frozen:
            raise AttributeError("Parameter {} cannot be changed in jobOptions.".format(self.name))
        else:
            self.__value = value
