#include <dpp/dpp.h>
#include <dpp/unicode_emoji.h>
#include <iostream>
#include <string.h>
#include <vector>
#include <format>
#include <ctime>
#include <iomanip>
//3h en este proyecto + lo puesto
int ch_num = 1;

struct project
{
	std::string ch_id;
	std::string Service_select;
	std::string budget = " ";
	std::string details = " ";
	dpp::user user;
};

std::string get_date(bool extra = false)
{
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    if (extra)
    	currentTime += 14 * 24 * 60 * 60;	
    std::stringstream ss;
    // Convert to formatted string using put_time
    ss << std::put_time(std::localtime(&currentTime), "%d/%m/%Y");
    return ss.str();
}

std::vector<project> projects;

std::string generate_description(project proj)
{
	dpp::role developers = *dpp::find_role(1141382494114041896);
	std::string formatted_str = std::format("**{}** has created an order!\n**Service:** {}\n**Budget:** ${}\n**Description:** {}\n\nOur team will be right with you! {}",
						proj.user.global_name, proj.Service_select, proj.budget, proj.details, developers.get_mention());
	return formatted_str;
}

std::string translate(std::string id)
{
	if (id == "Splugin")
	{
		return "Spigot Plugin";
	}
	else if (id == "Sdesign")
	{
		return "Server Design";
	}
	else if (id == "Dbot")
	{
		return "Discord Bot";
	}
	return " ";
}

int main()
{
	dpp::cluster bot("TOKEN");

    bot.on_log(dpp::utility::cout_logger());

   	 bot.on_slashcommand([&bot](const dpp::slashcommand_t & event) 
   	 {
        /* Check which command they ran */
        	if (event.command.get_command_name() == "finish_order") 
        	{
				std::string actual_date = get_date();
				std::string weeks_date = get_date(true);
				dpp::user_identified user = bot.user_get_sync(std::get<dpp::snowflake>(event.get_parameter("client")));
				std::string mention = user.get_mention();
				std::cout << mention << std::endl;
				dpp::embed embed = dpp::embed()
					.set_title("Order complete")
					.set_description(std::format("Hello! {},\nYour order is complete! Please provide your PayPal so an invoice can be sent! Order info:\n**Service:** {}\n**Price:** {}$\n**Completed Date:** {}\n**Payment Due Date:** {}\nThank you for your order! If the invoice is not paid by the due date, the project will be voided until further notice!", 
					mention ,std::get<std::string>(event.get_parameter("service")),std::get<long int>(event.get_parameter("price")),actual_date, weeks_date));
				dpp::message msg(event.command.channel_id, embed);
				event.reply(msg);
        	}
			else if (event.command.get_command_name() == "announce")
			{
				dpp::message msg(event.command.channel_id, std::get<std::string>(event.get_parameter("message")));
				
				bot.message_create(msg);
				event.reply(dpp::message("Message sent!").set_flags(dpp::m_ephemeral));
			}
			else if (event.command.get_command_name() == "close")
			{
				dpp::channel channel = event.command.channel;
				dpp::role developers = *dpp::find_role(1141382494114041896);
				channel.set_permission_overwrite(channel.guild_id, dpp::ot_role, 0, dpp::p_view_channel);
				channel.set_permission_overwrite(developers.id, dpp::ot_role, 0, dpp::p_view_channel);
				channel.set_permission_overwrite(719227090993414196, dpp::ot_member, dpp::p_view_channel, 0);
				bot.channel_edit(channel);
				event.reply(dpp::message("Order closed!").set_flags(dpp::m_ephemeral));
			}
    	});

    bot.on_ready([&bot](const dpp::ready_t & event) 
    {
        if (dpp::run_once<struct register_bot_commands>()) 
        {
            /* Create a new global command on ready event */
            dpp::slashcommand newcommand("finish_order", "Use when an order is finished", bot.me.id);
			newcommand.add_option(
				dpp::command_option(dpp::co_string, "service", "Select the service of the completed order", true)
					.add_choice(dpp::command_option_choice("spigot plugin", std::string("Spigot Plugin")))
					.add_choice(dpp::command_option_choice("server design", std::string("Server Design")))
					.add_choice(dpp::command_option_choice("discord bot", std::string("Discord Bot")))
			);
			newcommand.add_option(
				dpp::command_option(dpp::co_integer, "price", "Write the price of the completed order", bot.me.id)
			);
			newcommand.add_option(
				dpp::command_option(dpp::co_user, "client", "Select the client that ordered the service", bot.me.id)
			);

			dpp::slashcommand announce("announce", "use when you want the bot to send a message", bot.me.id);
			announce.add_option(
				dpp::command_option(dpp::co_string, "message", "The message you want the bot to send", true)
			);

			dpp::slashcommand close("close", "use when you want to close the order", bot.me.id);

	    	bot.global_command_create(newcommand);
			bot.global_command_create(announce);
			bot.global_command_create(close);
        }
        dpp::embed embed = dpp::embed()
            .set_title("Create order")
            .set_description("Click here to create an order!");
	    dpp::message msg(1155525606080000161, embed);
	    msg.add_component(
	    dpp::component().add_component(
            dpp::component()
                .set_label("Create order")
                .set_type(dpp::cot_button)
                .set_style(dpp::cos_primary)
                .set_id("create")
            )
        );
        bot.message_create(msg);
        std::cout << "bot ready!\n";
    });

    bot.on_button_click([&bot](const dpp::button_click_t & event) 
    {
       if (event.custom_id == "create")
       {
       	    std::cout << "Button clicked!\n";

       	    std::string ch_name;
	    	dpp::user user = event.command.get_issuing_user();
            ch_name = user.global_name;
            ch_name.append(" channel");
			dpp::guild guild = event.command.get_guild();
			dpp::role developers = *dpp::find_role(1141382494114041896);
			dpp::user client = event.command.get_issuing_user();
            dpp::channel new_ch = dpp::channel()
                .set_name(ch_name)
                .set_type(dpp::CHANNEL_TEXT)
                .set_guild_id(1141372279670976602)
				.set_permission_overwrite(guild.id, dpp::ot_role, 0, dpp::p_view_channel)
				.set_permission_overwrite(developers.id, dpp::ot_role, dpp::p_view_channel, 0)
				.set_permission_overwrite(client.id, dpp::ot_member, dpp::p_view_channel, 0)
				.set_permission_overwrite(bot.me.id, dpp::ot_member, dpp::p_view_channel, 0);
				
            dpp::channel actual_ch= bot.channel_create_sync(new_ch);
            dpp::embed embed = dpp::embed()
                .set_title("Terms of service")
                .set_description("Do you accept the terms of service listed here?\nhttps://flexibyte.dev/terms-of-service/");
            dpp::message ch_msg(actual_ch.id, embed);
            std::string id_name = "TOSaccept";
            id_name.append(actual_ch.id.str());
            ch_msg.add_component(
                dpp::component().add_component(
                dpp::component()
                    .set_label("Accept")
                    .set_type(dpp::cot_button)
                    .set_style(dpp::cos_success)
                    .set_id(id_name)
                    )
            );
            bot.message_create(ch_msg);
            ch_num++;
	    event.reply();
       }
       if (event.custom_id.starts_with("TOSaccept"))
       {
		    dpp::embed embed = dpp::embed()
			.set_description("Choose the service you want order")
			.set_title("Service");
            dpp::message msg(std::string(event.custom_id.c_str() + strlen("TOSaccept")), embed);
            std::string id_name = "Service_select";
            id_name.append(std::string(event.custom_id.c_str() + strlen("TOSaccept")));
            msg.add_component(
                dpp::component().add_component(
                    dpp::component()
                        .set_type(dpp::cot_selectmenu)
                        .set_placeholder("Select a service to order!")
                        .add_select_option(dpp::select_option("Spigot Plugin", "Splugin", "Do you want a spigot plugin for your server?"))
                        .add_select_option(dpp::select_option("Server Design", "Sdesign", "Do you want us to design your server?"))
                        .add_select_option(dpp::select_option("Discord Bot", "Dbot", "Do you want a discord bot for your server?"))
                        .set_id(id_name)
                )
            );

		bot.message_create(msg);
		event.reply();	
       }
    });
   
   bot.on_select_click([&bot](const dpp::select_click_t & event) 
    {
       if (event.custom_id.starts_with("Service_select"))
       {
       		std::cout << "Selected " << event.values[0] << std::endl;	
		std::string id = std::string(event.custom_id.c_str() + strlen("Service_select"));
		std::string Service_select = event.values[0];
		
		project new_project = {.ch_id = id, .Service_select = translate(event.values[0]), .user = event.command.get_issuing_user()};
		
		dpp::embed embed = dpp::embed()
			.set_description("Choose the budget you have for the service")
			.set_title("Budget");
		dpp::message msg(id, embed);
		std::string id_name = "Budget";
		id_name.append(id);
		msg.add_component(
			dpp::component().add_component(
				dpp::component()
					.set_type(dpp::cot_selectmenu)
					.set_placeholder("Select a budget!")
                        		.add_select_option(dpp::select_option("$0-49", "0-49", ""))
                        		.add_select_option(dpp::select_option("$50-99", "50-99", ""))
                        		.add_select_option(dpp::select_option("$100-249", "100-249", ""))
					.add_select_option(dpp::select_option("$250<", "250<", ""))
					.set_id(id_name)
                	)
                );
		bot.message_create(msg);
		projects.push_back(new_project);
		event.reply();
       }
       if (event.custom_id.starts_with("Budget"))
       {
       		std::cout << "Selected " << event.values[0] << std::endl;	
		std::string id = std::string(event.custom_id.c_str() + strlen("Budget"));
		std::string Service_select = event.values[0];
		
		for (int i = 0; i < projects.size();i++)
		{
			if (projects[i].ch_id.compare(id) == 0)
			{
				projects[i].budget = event.values[0];
				break;
			}
		}
		std::string id_name = "Description";
		id_name.append(id);	
		dpp::interaction_modal_response modal(id_name, "Description");

		modal.add_component(
			dpp::component()
				.set_label("Description of the service")
				.set_id("Desc")
				.set_type(dpp::cot_text)
                    		.set_placeholder("Describe the service you want!")
				.set_min_length(1)
				.set_max_length(2000)
				.set_text_style(dpp::text_paragraph)
		);
		event.dialog(modal);
       }
    });

	bot.on_form_submit([&bot](const dpp::form_submit_t & event) 
	{
		if (event.custom_id.starts_with("Description"))
		{
			std::string v = std::get<std::string>(event.components[0].components[0].value);
			std::string id = std::string(event.custom_id.c_str() + strlen("Description"));
			std::cout << "Full " << event.custom_id << "Id " << id << std::endl;
			project proj;
			for (int i = 0; i < projects.size();i++)
			{
				if (projects[i].ch_id.compare(id) == 0)
				{
					projects[i].details = v;
					proj = projects[i];
					break;
				}
			}
			dpp::embed embed = dpp::embed()
				.set_title("Summary")
				.set_description(generate_description(proj));
			dpp::message msg(id, embed);
			bot.message_create(msg);
		}
		event.reply(dpp::message("Saved! Thank you!").set_flags(dpp::m_ephemeral));
    	});

   bot.start(dpp::st_wait);
	 
}
