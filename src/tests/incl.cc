/*
 * Teng -- a general purpose templating engine.
 * Copyright (C) 2004  Seznam.cz, a.s.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Seznam.cz, a.s.
 * Naskove 1, Praha 5, 15000, Czech Republic
 * http://www.seznam.cz, mailto:teng@firma.seznam.cz
 *
 *
 * $Id: $
 *
 * DESCRIPTION
 * Teng engine -- basic tests.
 *
 * AUTHORS
 * Michal Bukovsky <michal.bukovsky@firma.seznam.cz>
 *
 * HISTORY
 * 2018-06-07  (burlog)
 *             Created.
 */

#include <teng.h>

#include "catch.hpp"
#include "utils.h"

SCENARIO(
    "The include directive",
    "[include]"
) {
    GIVEN("Template including empty.txt") {
        auto t = "<?teng include file='empty.txt'?>";

        WHEN("Generated with none data") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            auto result = g(err, t, root);

            THEN("It is empty string") {
                std::vector<Teng::Error_t::Entry_t> errs;
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "");
            }
        }
    }

    GIVEN("Template including text.txt") {
        auto t = "<?teng include file='text.txt'?>";

        WHEN("Generated with none data") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            auto result = g(err, t, root);

            THEN("It contains data from text.txt") {
                std::vector<Teng::Error_t::Entry_t> errs = {{
                    Teng::Error_t::WARNING,
                    {TEST_ROOT "text.txt", 1, 12},
                    "Runtime: Variable '.var' is undefined "
                    "[open_frags=., iteration=0]"
                }};
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "some text undefined\n");
            }
        }

        WHEN("Generated with variable defined") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            root.addVariable("var", "(var)");
            auto result = g(err, t, root);

            THEN("It contains data from text.txt") {
                std::vector<Teng::Error_t::Entry_t> errs;
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "some text (var)\n");
            }
        }
    }

    GIVEN("Template including text.txt with ignored options") {
        auto t = "<?teng include file='text.txt' some='ignored'?>";

        WHEN("Generated with variable defined") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            root.addVariable("var", "(var)");
            auto result = g(err, t, root);

            THEN("It contains data from text.txt") {
                std::vector<Teng::Error_t::Entry_t> errs;
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "some text (var)\n");
            }
        }
    }
}

SCENARIO(
    "The include directive in nested fragment",
    "[include]"
) {
    GIVEN("Template including empty.txt") {
        auto t = "<?teng frag sample?>{"
                 "<?teng include file='empty.txt'?>"
                 "}<?teng endfrag?>";

        WHEN("Generated with none data") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            auto result = g(err, t, root);

            THEN("It is empty string") {
                std::vector<Teng::Error_t::Entry_t> errs;
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "");
            }
        }

        WHEN("Generated with one fragment") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            root.addFragment("sample");
            auto result = g(err, t, root);

            THEN("It contains fragment data") {
                std::vector<Teng::Error_t::Entry_t> errs;
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "{}");
            }
        }
    }

    GIVEN("Template including text.txt") {
        auto t = "<?teng frag sample?>{"
                 "<?teng include file='text.txt'?>"
                 "}<?teng endfrag?>";

        WHEN("Generated with none data") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            auto result = g(err, t, root);

            THEN("It is empty string") {
                std::vector<Teng::Error_t::Entry_t> errs;
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "");
            }
        }

        WHEN("Generated with one fragment") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            root.addFragment("sample");
            auto result = g(err, t, root);

            THEN("It contains data from text.txt and fragment") {
                std::vector<Teng::Error_t::Entry_t> errs = {{
                    Teng::Error_t::WARNING,
                    {TEST_ROOT "text.txt", 1, 12},
                    "Runtime: Variable '.sample.var' is undefined "
                    "[open_frags=.sample, iteration=0]"
                }};
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "{some text undefined\n}");
            }
        }

        WHEN("Generated with variable defined") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            auto &sample = root.addFragment("sample");
            sample.addVariable("var", "(var)");
            auto result = g(err, t, root);

            THEN("It contains data from text.txt") {
                std::vector<Teng::Error_t::Entry_t> errs;
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "{some text (var)\n}");
            }
        }
    }
}

SCENARIO(
    "The include directive for missing file ",
    "[include]"
) {
    GIVEN("Template with bad include") {
        auto t = "<?teng include file='missing.txt'?>";

        WHEN("Generated with none data") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            auto result = g(err, t, root);

            THEN("It is empty string") {
                std::vector<Teng::Error_t::Entry_t> errs = {{
                    Teng::Error_t::ERROR,
                    {1, 0},
                    "Error reading file '" TEST_ROOT "missing.txt' "
                    "(No such file or directory)"
                }};
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "");
            }
        }
    }
}

SCENARIO(
    "The syntax error in include directive",
    "[include]"
) {
    GIVEN("Template with number as an option name") {
        auto t = "<?teng include 1='test.include.txt'?>";

        WHEN("Generated with none data") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            auto result = g(err, t, root);

            THEN("It is empty string") {
                std::vector<Teng::Error_t::Entry_t> errs = {{
                    Teng::Error_t::ERROR,
                    {1, 0},
                    "Invalid or excessive tokens in <?teng include?>; "
                    "ignoring them"
                }, {
                    Teng::Error_t::ERROR,
                    {1, 15},
                    "Unexpected token: name=DEC_INT, view=1"
                }};
                REQUIRE(err.getEntries() == errs);
                REQUIRE(result == "");
            }
        }
    }
}

SCENARIO(
    "Omitting duplicate errors across included files",
    "[include]"
) {
    GIVEN("Template with error and include with error") {
        auto t = "<?teng frag sample?>"
                 "here is missing variable before include ${before};"
                 "<?teng include file='incl.error.txt'?>;"
                 "and here, after the include ${after};"
                 "<?teng endfrag?>";

        WHEN("The template is rendered with more than one frag") {
            Teng::Error_t err;
            Teng::Fragment_t root;
            root.addFragment("sample");
            root.addFragment("sample");
            root.addFragment("sample");
            root.addFragment("sample");
            auto result = g(err, t, root);

            THEN("Then the errors should be in log only once") {
                std::vector<Teng::Error_t::Entry_t> errs = {{
                    Teng::Error_t::WARNING,
                    {1, 62},
                    "Runtime: Variable '.sample.before' is undefined "
                    "[open_frags=.sample, iteration=0]"
                }, {
                    Teng::Error_t::WARNING,
                    {1, 62},
                    "Runtime: Variable '.sample.before' is undefined "
                    "[open_frags=.sample, iteration=1]"
                }, {
                    Teng::Error_t::WARNING,
                    {1, 62},
                    "Runtime: Variable '.sample.before' is undefined "
                    "[open_frags=.sample, iteration=2]"
                }, {
                    Teng::Error_t::WARNING,
                    {1, 62},
                    "The 1 other error message(s) for this source code "
                    "position have been ignored"
                }, {
                    Teng::Error_t::WARNING,
                    {TEST_ROOT "incl.error.txt", 2, 2},
                    "Runtime: Variable '.sample.missing' is undefined "
                    "[open_frags=.sample, iteration=0]"
                }, {
                    Teng::Error_t::WARNING,
                    {TEST_ROOT "incl.error.txt", 2, 2},
                    "Runtime: Variable '.sample.missing' is undefined "
                    "[open_frags=.sample, iteration=1]"
                }, {
                    Teng::Error_t::WARNING,
                    {TEST_ROOT "incl.error.txt", 2, 2},
                    "Runtime: Variable '.sample.missing' is undefined "
                    "[open_frags=.sample, iteration=2]"
                }, {
                    Teng::Error_t::WARNING,
                    {TEST_ROOT "incl.error.txt", 2, 2},
                    "The 1 other error message(s) for this source code "
                    "position have been ignored"
                }, {
                    Teng::Error_t::WARNING,
                    {1, 139},
                    "Runtime: Variable '.sample.after' is undefined "
                    "[open_frags=.sample, iteration=0]"
                }, {
                    Teng::Error_t::WARNING,
                    {1, 139},
                    "Runtime: Variable '.sample.after' is undefined "
                    "[open_frags=.sample, iteration=1]"
                }, {
                    Teng::Error_t::WARNING,
                    {1, 139},
                    "Runtime: Variable '.sample.after' is undefined "
                    "[open_frags=.sample, iteration=2]"
                }, {
                    Teng::Error_t::WARNING,
                    {1, 139},
                    "The 1 other error message(s) for this source code "
                    "position have been ignored"
                }};
                REQUIRE(err.getEntries() == errs);
                std::string s = "here is missing variable before include "
                                "undefined;some\nundefined\ntext\n;"
                                "and here, after the include undefined;";
                REQUIRE(result == s + s + s + s);
            }
        }
    }
}

