RFCdown
=======

`RFCdown` is an experimental fork of [Hoedown](https://github.com/hoedown/hoedown), a revived fork of [Sundown](https://github.com/vmg/sundown) which was based on the original code of the [Upskirt library](http://fossil.instinctive.eu/libupskirt/index) by Natacha Porté. (Whew!)

The primary purpose of `RFCdown` is to support using Markdown to author IETF RFCs, using plain old standard C99.


Features
--------

*	**Fully standards compliant**

	`RFCdown` passes out of the box the official Markdown v1.0.0 and v1.0.3
	test suites, and has been extensively tested with additional corner cases
	to make sure its output is as sane as possible at all times.

*	**Massive extension support**

	`RFCdown` has optional support for several (unofficial) Markdown extensions,
	such as non-strict emphasis, fenced code blocks, tables, autolinks,
	strikethrough and more.

*	**UTF-8 aware**

	`RFCdown` is fully UTF-8 aware, both when parsing the source document and when
	generating the resulting (X)HTML code.

*	**Zero-dependency**

	`RFCdown` is a zero-dependency library composed of some `.c` files and their
	headers. No dependencies, no bullshit. Only standard C99 that builds everywhere.

*	**Additional features**

	`RFCdown` comes with a fully functional implementation of SmartyPants,
	a separate autolinker, escaping utilities, buffers, and stacks.

Unicode character handling
--------------------------

Given that the Markdown spec makes no provision for Unicode character handling, `RFCdown`
takes a conservative approach towards deciding which extended characters trigger Markdown
features:

*	Punctuation characters outside of the U+007F codepoint are not handled as punctuation.
	They are considered as normal, in-word characters for word-boundary checks.

*	Whitespace characters outside of the U+007F codepoint are not considered as
	whitespace. They are considered as normal, in-word characters for word-boundary checks.

Install
-------

Just typing `make` will build `RFCdown` the `rfcdown` executable, which is the command-line tool to render Markdown to HTML, RFC 2629 XML, and plain text.
