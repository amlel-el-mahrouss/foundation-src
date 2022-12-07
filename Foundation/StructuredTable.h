#pragma once

#include "Foundation.h"

namespace Foundation {
	class table final {
	public:
		table(std::istream& iobuf) {
			std::string buffer;
			std::getline(iobuf, buffer);

			if (buffer[0] != '{') {
				m_Values.push_back(buffer);
			} else {
				bool break_when{ false };

				for (size_t i = 0; i < buffer.size(); i += 1) {
					break_when = (i < buffer.size());
					while (break_when)
					{
						std::string tmp;
						int y = i;
						while (buffer[y] != ',' && buffer[y] != '}') {
							switch (buffer[y]) {
							case '{':
							case '\t':
							case '\r':
								++y; continue;
							case '!': 
							{
								tmp += ' ';
								++y;

								break;
							}
							default:
							{
								tmp += buffer[y];
								++y;

								break;
							}
							}

						}

						break_when = buffer[y] == '}';
						m_Values.emplace_back(std::move(tmp));

						i = y;
						break;
					}

					if (break_when) break;
				}
			}
		}

		~table() {}

		table& operator=(const table&) = default;
		table(const table&) = default;

		std::string operator[](const std::size_t& at) { return m_Values[at]; }

		auto try_find(const std::string& at) {
			auto found = std::find_if(m_Values.cbegin(), m_Values.cend(), [at](std::string elem) -> bool {
				return at == elem;
			});

			if (found == m_Values.cend())
				throw std::runtime_error("No such query.");

			return found;
		}

	private:
		std::vector<std::string> m_Values{  };

	};
}