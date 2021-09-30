/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: zdnaya <zdnaya@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/02 16:39:32 by zainabdnaya       #+#    #+#             */
/*   Updated: 2021/09/30 17:29:42 by zdnaya           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Webserv.hpp"

void check_server_keys(std::string key)
{
    if (key != "listen"  && key != "server_name" && key != "server_addr" && key != "root" && key != "error_page" && key != "client_body_size")
        throw std::runtime_error("Key " + key + " is not valid In server ");
}

void check_location_keys(std::string key)
{
    if (key != "index" && key != "location" && key != "http_methods" && key != "upload_path" && key != "root" && key != "autoindex" && key != "return" && key != "cgi_path")
        throw std::runtime_error("Key " + key + " is not valid In location");
}

int main(int ac, char *av[], char *envp[])
{
    if (ac != 2)
    {
        std::cerr << "Add conf file\n";
        exit(0);
    }
    if (compare_end(av[1], ".conf") == true)
    {
        try
        {
            ErrorHandling error(av[1]);
            Parsing *pars = new Parsing(av[1]);
            std::map<int, std::multimap<std::string, std::string> >::iterator it;
            std::multimap<std::string, std::string>::iterator it2;
            std::map<int, std::multimap<std::string, std::string> > tmp = pars->GetServerMap();
            for (it = tmp.begin(); it != tmp.end(); ++it)
            {
                // std::cout << YEL << "Server" << reset << std::endl;
                // std::cout << RED << "\t\tServer\t\t" << it->first << ":" << reset << std::endl;
                for (it2 = it->second.begin(); it2 != it->second.end(); ++it2)
                {
                    // std::cout << "\t|" << it2->first << "|\t\t\t|" << it2->second  << "|"<< "\n\n";
                    check_server_keys( it2->first);
                }
            }
            // std::cout << YEL << "\n\n";

            std::multimap<int, std::multimap<std::string, std::string> > tmp1 = pars->Getloc_map();
            std::multimap<int, std::multimap<std::string, std::string> >::iterator it3;
            std::multimap<std::string, std::string>::iterator it1;
            for (it3 = tmp1.begin(); it3 != tmp1.end(); ++it3)
            {
                // std::cout << YEL << "Location" << reset << std::endl;
                // std::cout << RED << "\t\tLocation\t\t" << it3->first << ":" << reset << std::endl;
                for (it1 = it3->second.begin(); it1 != it3->second.end(); ++it1)
                {
                    // std::cout << "\t|" << it1->first << "|\t\t\t|" << it1->second  << "|" << "\n\n";
                    check_location_keys(it1->first.substr(it1->first.find(" ")+1));
                }
            }
            // std::cout << YEL << "" << reset << std::endl;
            Server serv(pars, envp);
            delete pars;
        }

        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    else
        std::cerr << "wrong file \n";
    return (0);
}
