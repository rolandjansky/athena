# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

import curses.textpad


class Modal:
    def __init__(
        self, start_x: int, start_y: int, width: int, height: int
    ) -> None:
        self.width: int = width
        self.height: int = height
        self.start_x: int = start_x
        self.start_y: int = start_y
        self.window = curses.newwin(height, width, start_y, start_x)
        self.window.box()
        self.refresh()

    def refresh(self) -> None:
        self.window.refresh()

    def destroy(self) -> None:
        del self.window


class SearchModal(Modal):
    def __init__(
        self, start_x: int, start_y: int, width: int, height: int
    ) -> None:
        super().__init__(start_x, start_y, width, height)
        self.search_window = self.window.derwin(
            self.height - 2, self.width - 2, 1, 1
        )
        self.search = curses.textpad.Textbox(self.search_window)

    def edit(self) -> str:
        return self.search.edit().strip()


class HelpModal(Modal):
    def __init__(
        self, start_x: int, start_y: int, width: int, height: int
    ) -> None:
        super().__init__(start_x, start_y, width, height)
        self.help_window = self.window.derwin(
            self.height - 2, self.width - 2, 1, 1
        )

        self.begin_y, self.begin_x = self.window.getparyx()
        self.y, self.x = self.begin_y, self.begin_x
        self.y += 2
        self.x += 2

    def add_line(self, text: str, blank_lines: int = 1) -> None:
        self.window.addstr(self.y, self.x, text)
        self.y += blank_lines

    def show_help(self) -> None:
        pass


class SingleHelpModal(HelpModal):
    def show_help(self) -> None:

        self.add_line("Help", 2)

        self.add_line("Arrows for navigation", 2)

        self.add_line("h - display this help")
        self.add_line("m - search")
        self.add_line("space - mark/unmark")
        self.add_line("r - refresh the screen", 2)

        self.add_line("q - quit the program")

        self.refresh()


class DoubleHelpModal(HelpModal):
    def show_help(self) -> None:

        self.add_line("Help", 2)

        self.add_line("Arrows for navigation", 2)

        self.add_line("tab - change column")

        self.add_line("h - display this help")
        self.add_line("m - search")
        self.add_line("space - mark/unmark")
        self.add_line("r - refresh the screen", 2)

        self.add_line("q - quit the program")

        self.refresh()
