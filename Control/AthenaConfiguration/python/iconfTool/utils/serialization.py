# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
import logging
import pickle
from typing import Any, Set

logger = logging.getLogger(__name__)


def save_object(file_path: str, obj: Any) -> None:
    logger.info(f"Saving object of type {type(obj)} to {file_path}")
    with open(file_path, "wb") as f:
        pickle.dump(obj, f)


def load_strings_set(file_path: str) -> Set[str]:
    logger.info(f"Loading {file_path}")
    with open(file_path, "rb") as f:
        s: Set[str] = pickle.load(f)
    if not isinstance(s, set):
        msg = f"Saved object is not a set. Type: {type(s)}"
        logger.error(msg)
        raise TypeError(msg)
    for element in s:
        if not isinstance(element, str):
            msg = f"Set element is not a string. Type: {type(element)}"
            logger.error(msg)
            raise TypeError(msg)
    return s
