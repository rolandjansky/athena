/**
Copyright 2010-2015 Bernard van Gastel, bvgastel@bitpowder.com.
This file is part of Bit Powder Libraries.

Bit Powder Libraries is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Bit Powder Libraries is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Bit Powder Libraries.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef EXTENSION_H
#define EXTENSION_H

//#define EXTENSION_USING_CPP_TYPE_INDEX

#include "stack.h"
//#include "shared_object.h"

#ifdef EXTENSION_USING_CPP_TYPE_INDEX
#include <typeindex>
#else
#include "type_hash.h"
#endif

namespace bitpowder {
namespace lib {

template <class E, typename... CopyArgs>
class Extension {
  public:
#ifdef EXTENSION_USING_CPP_TYPE_INDEX
    std::type_index type;
#else
    FastTypeT type;
#endif
    E next = nullptr;
#ifdef EXTENSION_USING_CPP_TYPE_INDEX
    Extension() : type(typeid(E)) {}
#else
    Extension() : type(FastTypeEmpty) {}
#endif
    // one virtual method to make dynamic_cast work
    virtual ~Extension() {}

    E copy(CopyArgs&& ... args) {
        return nullptr;
    }
};

template <class E, typename... CopyArgs>
class ExtensionContainer {
  public:
    typedef E Element;
    typedef Stack<Element, Extension<E,CopyArgs...>> ExtensionStack;
  private:
    template <class T>
    class Match {
#ifdef EXTENSION_USING_CPP_TYPE_INDEX
        std::type_index targetType;
#endif
      public:
#ifdef EXTENSION_USING_CPP_TYPE_INDEX
        Match() : targetType(typeid(std::remove_pointer<T>::type)) {
#else
        Match() {
#endif
        }
        std::pair<bool,T> operator()(Extension<E,CopyArgs...>* e) {
#ifdef EXTENSION_USING_CPP_TYPE_INDEX
            return {e->type == targetType, static_cast<T>(e)};
#else
            return {FastType<typename std::remove_pointer<T>::type>::pointer_of_type(e), static_cast<T>(e)};
#endif
        }
    };
    template <class T>
    class MatchDynamic {
      public:
        std::pair<bool,T> operator()(Element e) {
            auto retval = dynamic_cast<T>(&*e); // extra &* is used to get rid of smart-containers like lib::shared_object
            return {retval != nullptr, retval};
        }
    };
  protected:
    ExtensionStack extensions;
  public:
    ExtensionContainer() {
    }

    ExtensionContainer(const ExtensionContainer& c) {
        // otherwise wrong order will be used, can matter
        ExtensionStack toBeAdded;
        for (Element ext : c.extensions) {
            Element copy = ext->copy();
            if (copy)
                toBeAdded.push(copy);
        }
        while (!toBeAdded.empty())
            extensions.push(toBeAdded.pop());
    }

    ExtensionContainer(ExtensionContainer&& c) = default;
    ExtensionContainer& operator=(ExtensionContainer&& c) = default;

    ExtensionContainer copy(const ExtensionContainer& c, CopyArgs&& ... args) {
        // otherwise wrong order will be used, can matter
        ExtensionContainer retval;
        ExtensionStack toBeAdded;
        for (Element ext : c.extensions) {
            Element copy = ext->copy(std::forward<CopyArgs...>(args...));
            if (copy)
                toBeAdded.push(copy);
        }
        while (!toBeAdded.empty())
            retval.extensions.push(toBeAdded.pop());
        return retval;
    }

    ExtensionStack& getAllExtensions() {
        return extensions;
    }

    template <class T>
    T getExtension() const {
        return extensions.first(Match<T>());
    }

    template <class T>
    T getExtensionOfBaseType() const {
        return extensions.first(MatchDynamic<T>());
    }

    template <class T>
    auto selectExtensions() const -> decltype(extensions.select(std::declval<Match<T>&&>())) {
        return extensions.template select(Match<T>());
    }
    
    template <class T>
    auto selectExtensions() -> decltype(extensions.select(std::declval<Match<T>&&>())) {
        return extensions.template select(Match<T>());
    }

    template <class T>
    auto selectExtensionsOfBaseType() const -> decltype(extensions.select(std::declval<MatchDynamic<T>&&>())) {
        return extensions.template select(MatchDynamic<T>());
    }
    
    template <class T>
    auto selectExtensionsOfBaseType() -> decltype(extensions.select(std::declval<MatchDynamic<T>&&>())) {
        return extensions.template select(MatchDynamic<T>());
    }

    template <class T>
    void addExtension(const T& extension) {
#ifdef EXTENSION_USING_CPP_TYPE_INDEX
        extension->type = std::type_index(typeid(typename std::remove_pointer<T>::type));
#else
        //extension->type = FastType<typename std::remove_pointer<T>::type>::value;
        extension->Extension<E>::type = FastType<typename std::remove_pointer<T>::type>::value;
#endif
        extensions.push_back(extension);
    }

    template <class T>
    T removeExtension() {
        Match<T> m;
        for (auto it = extensions.select(std::ref(m)).begin(); it != extensions.end(); )
            return it.erase();
        return nullptr;
    }

    template <class T>
    T removeExtensionOfBaseType() {
        MatchDynamic<T> m;
        for (auto it = extensions.select(std::ref(m)).begin(); it != extensions.end(); )
            return it.erase();
        return nullptr;
    }

    ExtensionStack clearExtensions() {
        return extensions.pop_all();
    }
};

}
}

#endif // EXTENSION_H
